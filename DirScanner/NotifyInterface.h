#pragma once

#include "common.h"

class SubscriberBase {
public:
	virtual void OnScanOneStart(ScanInfo info) = 0;
	virtual void OnScanOneFinish(ScanInfo info) = 0;
	virtual void OnScanAllFinish(ScanInfo info) = 0;
};
