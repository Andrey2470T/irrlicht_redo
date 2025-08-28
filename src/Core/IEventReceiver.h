#pragma once

namespace core
{

class Event;

//! Interface of an object which can receive events.
/** Many of the engine's classes inherit IEventReceiver so they are able to
process events. Events usually start at a postEventFromUser function and are
passed down through a chain of event receivers until OnEvent returns true. See
irr::EEVENT_TYPE for a description of where each type of event starts, and the
path it takes through the system. */
class IEventReceiver
{
public:
	//! Destructor
	virtual ~IEventReceiver() {}

	//! Called if an event happened.
	/** Please take care that you should only return 'true' when you want to _prevent_ Irrlicht
	* from processing the event any further. So 'true' does mean that an event is completely done.
	* Therefore your return value for all unprocessed events should be 'false'.
	\return True if the event was processed.
	*/
    virtual bool OnEvent(const Event &event) = 0;
};

}
