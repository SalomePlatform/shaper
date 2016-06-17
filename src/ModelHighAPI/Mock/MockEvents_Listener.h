// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef MockEvents_Listener_H_
#define MockEvents_Listener_H_

#include <gmock/gmock.h>

#include <Events_Listener.h>

class MockEvents_Listener : public Events_Listener {
public:
  MOCK_METHOD1(processEvent,
    void(const std::shared_ptr<Events_Message>& theMessage));
};

#endif // MockEvents_Listener_H_
