// This file is part of Projecteur - https://github.com/jahnf/projecteur - See LICENSE.md and README.md
#pragma once

#include <QObject>

#include <map>
#include <memory>

#include "enum-helper.h"
#include "devicescan.h"

class QTimer;
class Settings;
class VirtualDevice;

/// Class handling spotlight device connections and indicating if a device is sending
/// sending mouse move events.
class Spotlight : public QObject
{
  Q_OBJECT

public:
  struct Options {
    bool enableUInput = true; // enable virtual uinput device
    QList<SupportedDevice> additionalDevices;
  };

  explicit Spotlight(QObject* parent, Options options, Settings* settings);
  virtual ~Spotlight();

  bool spotActive() const { return m_spotActive; }

  struct ConnectedDeviceInfo {
    DeviceId id;
    QString name;
  };

  bool anySpotlightDeviceConnected() const;
  uint32_t connectedDeviceCount() const;
  QList<ConnectedDeviceInfo> connectedDevices() const;
  std::shared_ptr<DeviceConnection> deviceConnection(const DeviceId& deviceId);

signals:
  void deviceConnected(const DeviceId& id, const QString& name);
  void deviceDisconnected(const DeviceId& id, const QString& name);
  void subDeviceConnected(const DeviceId& id, const QString& name, const QString& path);
  void subDeviceDisconnected(const DeviceId& id, const QString& name, const QString& path);
  void anySpotlightDeviceConnectedChanged(bool connected);
  void spotActiveChanged(bool isActive);

private:
  enum class ConnectionResult { CouldNotOpen, NotASpotlightDevice, Connected };
  ConnectionResult connectSpotlightDevice(const QString& devicePath, bool verbose = false);

  bool addInputEventHandler(std::shared_ptr<SubEventConnection> connection);

  bool setupDevEventInotify();
  int connectDevices();
  void removeDeviceConnection(const QString& devicePath);
  void onEventDataAvailable(int fd, SubEventConnection& connection);

  const Options m_options;
  std::map<DeviceId, std::shared_ptr<DeviceConnection>> m_deviceConnections;

  QTimer* m_activeTimer = nullptr;
  QTimer* m_connectionTimer = nullptr;
  bool m_spotActive = false;
  std::shared_ptr<VirtualDevice> m_virtualDevice;
  Settings* m_settings = nullptr;
};
