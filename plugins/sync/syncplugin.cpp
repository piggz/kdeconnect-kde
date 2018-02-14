/**
 * Copyright 2015 Ashish Bansal <bansal.ashish096@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "syncplugin.h"

#include <KPluginFactory>
#include <KJobTrackerInterface>

#include <QDebug>
#include <QDBusConnection>
#include <QLoggingCategory>
#include <QCryptographicHash>
#include <QUrl>
#include <QStandardPaths>
#include <QDirIterator>
#include <QDir>
#include <QDateTime>

#include <core/device.h>
#include <core/filetransferjob.h>

K_PLUGIN_FACTORY_WITH_JSON( KdeConnectPluginFactory, "kdeconnect_sync.json", registerPlugin< SyncPlugin >(); )

Q_LOGGING_CATEGORY(KDECONNECT_PLUGIN_SYNC, "kdeconnect.plugin.sync")

SyncPlugin::SyncPlugin(QObject* parent, const QVariantList& args)
    : KdeConnectPlugin(parent, args)
{
    qCDebug(KDECONNECT_PLUGIN_SYNC) << "Sync plugin constructor for device" << device()->name();
}

SyncPlugin::~SyncPlugin()
{
    qCDebug(KDECONNECT_PLUGIN_SYNC) << "Sync plugin destructor for device" << device()->name();
}

bool SyncPlugin::receivePackage(const NetworkPackage& np)
{
    qCDebug(KDECONNECT_PLUGIN_SYNC) << np.hasPayload() << np.has("relativePaths") << np.has("sendfiles");
    if (np.hasPayload()) {
        receiveFile(np);
    } else if (np.has("relativePaths")) {
        compareAndRequestFiles(np);
    } else if (np.has("sendfiles")) {
        sendRequestedFiles(np);
    }

    return true;
}

void SyncPlugin::receiveFile(const NetworkPackage& np)
{
    const QString filepath = np.get<QString>("filepath");
    if (filepath.isEmpty()) {
        qCDebug(KDECONNECT_PLUGIN_SYNC) << "Filepath empty in Network Package";
        return;
    }

    qCDebug(KDECONNECT_PLUGIN_SYNC)<< "Going to receive file :" << filepath;
    QFileInfo f(m_syncDir.filePath(filepath));
    if (f.exists()) {
        QFile file(f.absoluteFilePath());
        file.remove();
    }

    if (!m_syncDir.mkpath(f.dir().path())) {
        qCDebug(KDECONNECT_PLUGIN_SYNC) << "Failed Creating Directory" << f.dir().path();
        return;
    }

    FileTransferJob* job = np.createPayloadTransferJob(QUrl(f.absoluteFilePath()));
    connect(job, SIGNAL(result(KJob*)), this, SLOT(finished(KJob*)));
    KIO::getJobTracker()->registerJob(job);
    job->start();
    m_incomingFiles.append(filepath);
}

void SyncPlugin::compareAndRequestFiles(const NetworkPackage &np)
{
    QStringList fileList = listFilesInDirectory();

    QList<FileProperties> localFilesProperties = getFilesDesc(fileList);
    QStringList localHashes, localRelativePaths, localModifiedDates;
    Q_FOREACH(FileProperties f, localFilesProperties) {
        localHashes.append(f.hash);
        localRelativePaths.append(f.relativePath);
        localModifiedDates.append(f.modifiedDate);
    }

    QStringList remoteRelativePaths = np.get<QStringList>("relativePaths");
    QStringList remoteHashes = np.get<QStringList>("hashes");
    QStringList remoteModifiedDates = np.get<QStringList>("modifiedDates");

    QStringList filelist;
    for (int i = 0; i < remoteRelativePaths.size(); i++) {
        QString remoteFilePath = remoteRelativePaths.at(i);
        QString remoteHash = remoteHashes.at(i);
        QDateTime remoteModifiedDate = QDateTime::fromString(remoteModifiedDates.at(i));

        // Check if we already are receiving the file and if yes, then don't request that file again.
        if (m_incomingFiles.contains(remoteFilePath)) {
            continue;
        }

        int index = localRelativePaths.indexOf(remoteFilePath);
        if (index == -1) {
            filelist.append(remoteFilePath);
            continue;
        }

        QDateTime localModifiedDate = QDateTime::fromString(localModifiedDates.at(index));
        if (remoteModifiedDate > localModifiedDate) {
            QString localHash = localHashes.at(index);
            if (remoteHash != localHash) {
                filelist.append(remoteFilePath);
            }
        }
    }

    qCDebug(KDECONNECT_PLUGIN_SYNC) << "Requested Files" << filelist;
    if (filelist.isEmpty()) {
        return;
    }

    NetworkPackage sendFilesNp(PACKAGE_TYPE_SYNC);
    sendFilesNp.set<QStringList>("sendfiles", filelist);
    sendPackage(sendFilesNp);
}

void SyncPlugin::sendRequestedFiles(const NetworkPackage &np)
{
    QStringList filelist = np.get<QStringList>("sendfiles");
    qCDebug(KDECONNECT_PLUGIN_SYNC) << "Sending following files :" << filelist;
    Q_FOREACH (QString f, filelist) {
        QUrl url = QUrl::fromLocalFile(m_syncDir.filePath(f));
        NetworkPackage package(PACKAGE_TYPE_SYNC);
        QSharedPointer<QIODevice> ioFile(new QFile(url.toLocalFile()));
        package.setPayload(ioFile, ioFile->size());
        package.set<QString>("filepath", f);
        sendPackage(package);
    }
}

void SyncPlugin::finished(KJob* job)
{
    qCDebug(KDECONNECT_PLUGIN_SYNC) << "File transfer finished. Success:" << (!job->error());
    FileTransferJob *filetransferjob = static_cast<FileTransferJob*>(job);
    QString path = filetransferjob->destination().toLocalFile();
    QString relativePath = m_syncDir.relativeFilePath(path);
    m_incomingFiles.removeAll(relativePath);
    startSyncing();
}

void SyncPlugin::connected()
{
    QDBusConnection::sessionBus().registerObject(dbusPath(), this, QDBusConnection::ExportAllContents);

    // ToDo: use PluginConfig to store and load sync directory
    if (!QDir::home().mkpath("sync")) {
        return;
    }
    setSyncDirectory(QDir::homePath() + "/sync/");

    syncDirWatcher.addPath(m_syncDir.absolutePath());
    connect(&syncDirWatcher, &QFileSystemWatcher::directoryChanged, this, &SyncPlugin::directoryChanged);
    connect(&syncDirWatcher, &QFileSystemWatcher::fileChanged, this, &SyncPlugin::startSyncing);
    startSyncing();
}

void SyncPlugin::directoryChanged(QString path)
{
    if (!QFile::exists(path)) {
        QDirIterator it(path, QDir::NoDotAndDotDot | QDir::Dirs, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            syncDirWatcher.addPath(it.next());
        }
    }

    startSyncing();
}

void SyncPlugin::startSyncing()
{
    QStringList fileList = listFilesInDirectory();
    QList<FileProperties> filesDesc = getFilesDesc(fileList);
    if (filesDesc.isEmpty()) {
        return;
    }

    QStringList hashes, relativePaths, modifiedDates;
    Q_FOREACH(auto f, filesDesc) {
        hashes.append(f.hash);
        relativePaths.append(f.relativePath);
        modifiedDates.append(f.modifiedDate);
    }

    NetworkPackage np(PACKAGE_TYPE_SYNC);
    np.set("hashes", hashes);
    np.set("relativePaths", relativePaths);
    np.set("modifiedDates", modifiedDates);
    sendPackage(np);
    qCDebug(KDECONNECT_PLUGIN_SYNC) << "Local files information send!" << relativePaths;
}

QString SyncPlugin::syncDirectory()
{
    return m_syncDir.absolutePath();
}

bool SyncPlugin::setSyncDirectory(QString path)
{
    if (path.isEmpty()) {
        qCDebug(KDECONNECT_PLUGIN_SYNC) << "Passed empty path";
        return false;
    }

    if (QDir::isRelativePath(path)) {
        qCDebug(KDECONNECT_PLUGIN_SYNC) << "Found relative path, supports only absolute path";
        return false;
    }

    QDir dir(path);
    if (!dir.exists() && !dir.mkpath(path)) {
        qCDebug(KDECONNECT_PLUGIN_SYNC) << "Unable to create specified directory";
        return false;
    }

    m_syncDir.setPath(path);
    return true;
}

QStringList SyncPlugin::listFilesInDirectory()
{
    QStringList files;
    QDirIterator it(syncDirectory(), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString file = it.next();
        if (!file.endsWith(".part")) {
            syncDirWatcher.addPath(file);
            files.append(file);
        }
    }
    qCDebug(KDECONNECT_PLUGIN_SYNC) << "Local sync directory files" << files;
    return files;
}

QList<SyncPlugin::FileProperties> SyncPlugin::getFilesDesc(QStringList& fileList)
{
    QList<FileProperties> filesDesc;
    Q_FOREACH(QString filename, fileList) {
        QFile file(filename);
        QString relativePath = m_syncDir.relativeFilePath(filename);
        if (relativePath.isEmpty()) {
            continue;
        }
        if (file.open(QFile::ReadOnly)) {
            QCryptographicHash hash(QCryptographicHash::Md5);
            QFileInfo fileInfo(filename);
            if (hash.addData(&file)) {
                FileProperties f;
                f.relativePath = relativePath;
                f.hash = QString(hash.result().toHex());
                f.modifiedDate = fileInfo.lastModified().toString();
                filesDesc.append(f);
            }
        }
    }
    return filesDesc;
}

QString SyncPlugin::dbusPath() const
{
    return "/modules/kdeconnect/devices/" + device()->id() + "/sync";
}

#include "syncplugin.moc"
