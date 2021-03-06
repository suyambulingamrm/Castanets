// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/media/router/discovery/discovery_network_monitor_metric_observer.h"

namespace media_router {
namespace {

// This TimeDelta indicates the amount of time to wait between receiving a
// network ID of kNetworkIdDisconnected in a change event and checking that we
// are actually disconnected and reporting it.
//
// net::NetworkChangeNotifier, and therefore DiscoveryNetworkMonitor, reports a
// disconnected state between any other network change.  In order to avoid
// reporting these spurious kNetworkIdDisconnected events as real
// disconnections, we wait for this timeout and then check the network status
// again.  If any other network change notification comes while we are waiting,
// this state check is cancelled.
constexpr base::TimeDelta kConfirmDisconnectTimeout =
    base::TimeDelta::FromSeconds(1);

DiscoveryNetworkMonitorConnectionType ConnectionTypeFromIdAndType(
    const std::string& network_id,
    net::NetworkChangeNotifier::ConnectionType connection_type) {
  if (network_id == DiscoveryNetworkMonitor::kNetworkIdDisconnected) {
    return DiscoveryNetworkMonitorConnectionType::kDisconnected;
  } else if (network_id == DiscoveryNetworkMonitor::kNetworkIdUnknown) {
    switch (connection_type) {
      case net::NetworkChangeNotifier::CONNECTION_WIFI:
        return DiscoveryNetworkMonitorConnectionType::kUnknownReportedAsWifi;
      case net::NetworkChangeNotifier::CONNECTION_ETHERNET:
        return DiscoveryNetworkMonitorConnectionType::
            kUnknownReportedAsEthernet;
      case net::NetworkChangeNotifier::CONNECTION_UNKNOWN:
        return DiscoveryNetworkMonitorConnectionType::kUnknown;
      default:
        return DiscoveryNetworkMonitorConnectionType::kUnknownReportedAsOther;
    }
  } else {
    switch (connection_type) {
      case net::NetworkChangeNotifier::CONNECTION_WIFI:
        return DiscoveryNetworkMonitorConnectionType::kWifi;
      case net::NetworkChangeNotifier::CONNECTION_ETHERNET:
        return DiscoveryNetworkMonitorConnectionType::kEthernet;
      default:
        return DiscoveryNetworkMonitorConnectionType::kUnknown;
    }
  }
}

}  // namespace

DiscoveryNetworkMonitorMetricObserver::DiscoveryNetworkMonitorMetricObserver(
    std::unique_ptr<base::TickClock> tick_clock,
    std::unique_ptr<DiscoveryNetworkMonitorMetrics> metrics)
    : tick_clock_(std::move(tick_clock)),
      metrics_(std::move(metrics)),
      disconnect_timer_(tick_clock_.get()) {
  DCHECK(tick_clock_);
  DCHECK(metrics_);
}

DiscoveryNetworkMonitorMetricObserver::
    ~DiscoveryNetworkMonitorMetricObserver() {}

void DiscoveryNetworkMonitorMetricObserver::OnNetworksChanged(
    const std::string& network_id) {
  auto now = tick_clock_->NowTicks();
  if (network_id == DiscoveryNetworkMonitor::kNetworkIdDisconnected) {
    disconnect_timer_.Start(FROM_HERE, kConfirmDisconnectTimeout,
                            base::Bind(&DiscoveryNetworkMonitorMetricObserver::
                                           ConfirmDisconnectedToReportMetrics,
                                       base::Unretained(this), now));
    return;
  } else if (last_event_time_) {
    metrics_->RecordTimeBetweenNetworkChangeEvents(now - *last_event_time_);
  }
  last_event_time_ = now;
  disconnect_timer_.Stop();
  DiscoveryNetworkMonitorConnectionType connection_type =
      ConnectionTypeFromIdAndType(
          network_id, net::NetworkChangeNotifier::GetConnectionType());
  metrics_->RecordConnectionType(connection_type);
}

void DiscoveryNetworkMonitorMetricObserver::ConfirmDisconnectedToReportMetrics(
    base::TimeTicks disconnect_time) {
  if (last_event_time_) {
    metrics_->RecordTimeBetweenNetworkChangeEvents(disconnect_time -
                                                   *last_event_time_);
  }
  last_event_time_ = disconnect_time;
  metrics_->RecordConnectionType(
      DiscoveryNetworkMonitorConnectionType::kDisconnected);
}

}  // namespace media_router
