#include <cassert>
#include <cstdint>
#include <vector>
/* #include <queue> */
/* #include <list> */
#include <limits>

#include "interface.h"
#include "queue.h"
#include "forward_list.h"

using std::size_t;
using std::vector;
/* using std::queue; */
/* using std::list; */
template <typename T>
using list = forward_list<T>;

/// This is sample empty implementation you can place your solution here or delete this and include tests to your solution

// ???? clients that can take what they want will leave in order of arrival?? - yes
// ???? first clients that can take what they want leave or clients that have waited their limit - in order of index
// !!!! clients may be added before the time has been advanced to their arrival time
// !!!! assumption: don't need to send multiple restock missions for a single client 
// (otherwise the rule becomes, send restock even if not necessary, but don't send any unnecessary restocks above those that have been already sent that were necessary)
// !!!! assumption: don't need to send restocks for old clients when new clients arrive to make up for what the new client will steal from the old one

struct RestockingMission {
    int minute;
    ResourceType type;
    int leftoverQuantity;
    //the restock quantity itself is always the same
    //however we need to know how much from the restock quantity will be left over for 
    //other clients that are yet to arrive that haven't triggered the restock
};

struct ClientWrapper {
    ClientWrapper(int index, const Client &client) : index(index), arriveMinute(client.arriveMinute), banana(client.banana), schweppes(client.schweppes), maxWaitTime(client.maxWaitTime) {}
    int index;
    int arriveMinute;
    int banana;
    int schweppes;
    int maxWaitTime;
};

struct Event {
	enum Type {
		None, WorkerSend, WorkerBack, ClientArrive, ClientDepart
	};

	Type type;
	int minute;

	struct Worker {
		ResourceType resource;
	} worker;

	struct Client {
		int index;
		int banana;
		int schweppes;
	} client;
};

///below class's purpose is to abstract the logic for the order of event dispatch specified in the assignment
///does so by not flushing events that happen at the same time until an event at a later time comes
///and then flushes them not in the order it got them in but in the desired order from the assignment
///ensures correct order of events dispatch
///used in the class implementation, does't not interfere with the interface!!!!!!
///assumes events will be passed in order of occurrence minute
///assumes there will not be any Event::None or Event::ClientArrive events sent
///assumes banana restocks are passed before schweppes restocks
///assumes that you will call flush when you know that there will be no more events in the same minute
class ActionHandlerWrapper { 
public:
    ActionHandlerWrapper(ActionHandler *handler) : handler(handler) {}

    void onWorkerSend(int minute, ResourceType resource) {
        if (checkAutomaticFlush(minute)) {
            flush();
        }
        unflushedWorkerSend.push({Event::Type::WorkerSend, minute, {resource}});
    }

    void onWorkerBack(int minute, ResourceType resource) {
        if (checkAutomaticFlush(minute)) {
            flush();
        }
        unflushedWorkerBack.push({Event::Type::WorkerBack, minute, {resource}});
    }

    void onClientDepart(int index, int minute, int banana, int schweppes) {
        if (checkAutomaticFlush(minute)) {
            flush();
        }
        unflushedClientDepart.push({Event::Type::ClientDepart, minute, {}, {index, banana, schweppes}});
    }

    void flush() {
        while (!unflushedWorkerSend.empty() || !unflushedWorkerBack.empty() || !unflushedClientDepart.empty()) { //assumes only events at equal minute are unflushed
            auto &q = (!unflushedWorkerSend.empty())? unflushedWorkerSend: (!unflushedWorkerBack.empty())? unflushedWorkerBack: unflushedClientDepart;
            const auto &ev = q.front();
            if (ev.type == Event::Type::WorkerSend) {
                handler->onWorkerSend(ev.minute, ev.worker.resource);
            } else if (ev.type == Event::Type::WorkerBack) {
                handler->onWorkerBack(ev.minute, ev.worker.resource);
            } else {
                handler->onClientDepart(ev.client.index, ev.minute, ev.client.banana, ev.client.schweppes);
            }
            q.pop();
        }
    }
private:
    ActionHandler *handler;
    queue<Event> unflushedWorkerSend;
    queue<Event> unflushedWorkerBack;
    queue<Event> unflushedClientDepart;

    bool checkAutomaticFlush(int eventMinute) const {
        if (!unflushedWorkerSend.empty() && eventMinute > unflushedWorkerSend.front().minute) 
            return true;
        if (!unflushedWorkerBack.empty() && eventMinute > unflushedWorkerBack.front().minute)
            return true;
        if (!unflushedClientDepart.empty() && eventMinute > unflushedClientDepart.front().minute)
            return true;
        return false;
    }
};



class MyStore : public Store {
public:
	void setActionHandler(ActionHandler *handler) override {
		actionHandler = new ActionHandlerWrapper(handler);
	}

	void init(int workerCount, int startBanana, int startSchweppes) override {
        minute = 0;
        availableWorkers = workerCount;
        banana = startBanana;
        schweppes = startSchweppes;
        nextClientIndex = 0;
	}

	void addClients(const Client *clients, int count) override {
        for (size_t i=0; i<count; ++i) {
            arrivingClients.push(ClientWrapper(
                nextClientIndex + i,
                {clients[i]}
            ));
            if (i > 0) {
                assert(clients[i-1].arriveMinute <= clients[i].arriveMinute);
            }
        }
        nextClientIndex = count;
	}
	void advanceTo(int minute) override {
        assert(minute >= this->minute);
        //send, arrive restock, sell
        //below loop executes all events up to that minute in their order of occurrence
        //executes events from the same minute in the following order: clients arrive, restocks send, restocks arrive, clients depart, 
        //wrapper of actionHandler will ensure the order is according to the assignment
        for (Event::Type eventType = nextEvent(minute); this->minute <= minute && eventType != Event::Type::None; eventType = nextEvent(minute)) {
            if (eventType == Event::Type::ClientArrive) {
                assert(!arrivingClients.empty());
                const auto &client = arrivingClients.front();
                this->minute = client.arriveMinute; //above or below addClient
                addClient(client); // it implicitly takes into account subsequent events from the loop when deciding whether to send a worker
                arrivingClients.pop();
            } else if (eventType == Event::Type::ClientDepart) {
                auto it = getLeavingClientIT();
                assert(it != waitingClients.end());
                const auto &client = *it;
                int leaveTime = getLeaveTime(client);
                int takeBanana = std::min(banana, client.banana);
                int takeSchweppes = std::min(schweppes, client.schweppes);
                banana    -= takeBanana;
                schweppes -= takeSchweppes;
                actionHandler->onClientDepart(client.index, leaveTime, takeBanana, takeSchweppes);
                this->minute = leaveTime;
                waitingClients.erase(it); 
            } else {
                assert(!pendingRestocks.empty());
                assert(eventType == Event::Type::WorkerBack);
                const auto &restock = pendingRestocks.front();
                if (restock.type == ResourceType::banana) {
                    banana += 100;
                } else {
                    assert(restock.type == ResourceType::schweppes);
                    schweppes += 100;
                }
                actionHandler->onWorkerBack(restock.minute, restock.type);
                ++availableWorkers;
                this->minute = restock.minute;
                pendingRestocks.pop();
            }
        }
        //restocks arrive
        //clients arrive
        //clients leave
        //restocks send

        actionHandler->flush();
	}

	int getBanana() const override {
		return banana;
	}

	int getSchweppes() const override {
		return schweppes;
	}

private:
    Event::Type nextEvent(int minute) const {
        Event::Type ans = Event::Type::None;
        // keep in mind it is possible to have clients have maxWaitTime=0
        if (!waitingClients.empty()) {
            const auto res = getLeavingClientIT();
            if (res != waitingClients.end()) {
                const auto &leavingClient = *res;
                int leaveMinute = getLeaveTime(leavingClient); // client leaves now if he has enough or waits till limit
                if (leaveMinute <= minute) {
                    minute = leaveMinute;
                    ans = Event::Type::ClientDepart;
                }
            } 
        }
        //pending restocks must override leaving clients if possible
        if (!pendingRestocks.empty() && pendingRestocks.front().minute <= minute) {
            minute = pendingRestocks.front().minute;
            ans = Event::Type::WorkerBack;
        }
        //arriving clients must override pending restocks if possible
        if (!arrivingClients.empty() && arrivingClients.front().arriveMinute <= minute) {
            ans = Event::Type::ClientArrive;
        }
        return ans;
    }

    //This action handler tracks how much of each resource any particular customer bought
    struct TrackClientActionHandler : ActionHandler {
        explicit TrackClientActionHandler(int trackIndex) : trackIndex(trackIndex) {}
        void onClientDepart(int index, int minute, int banana, int schweppes) override {
            if (index == trackIndex) {
                boughtBanana = banana;
                boughtSchweppes = schweppes;
            }
        }
        void onWorkerSend(int minute, ResourceType resource) override {}
        void onWorkerBack(int minute, ResourceType resource) override {}

        int trackIndex;
        int boughtBanana = 0;
        int boughtSchweppes = 0;
        /* int boughtBanana() {} */
        /* int boughtSchweppes() {} */
    };
    void addClient(const ClientWrapper &client) {
        waitingClients.push_back(client);
        //check if he leaves unsatisfied and if it can even be rectified
        // we need to copy the store on every step
        // so that we take into account whether
        // previous clients will take from the additional restocked quantity
        MyStore alternateStore = *this;
        TrackClientActionHandler tracker(client.index);
        alternateStore.setActionHandler(&tracker);
        // we can't have foresight for clients that have not yet arrived
        // and also don't want infinite recursion:
        alternateStore.arrivingClients = {}; 
        alternateStore.advanceTo(client.arriveMinute + client.maxWaitTime);
        int bananaShortage = client.banana - tracker.boughtBanana;
        int schweppesShortage = client.schweppes - tracker.boughtSchweppes;
        if (availableWorkers && (bananaShortage > 0 || schweppesShortage > 0)) {
            if (bananaShortage >= schweppesShortage) { // if equal sends banana restocks first
                pendingRestocks.push({
                    this->minute+60, 
                    ResourceType::banana
                });
                actionHandler->onWorkerSend(this->minute, ResourceType::banana);
            } else {
                pendingRestocks.push({
                    this->minute+60, 
                    ResourceType::schweppes
                });
                actionHandler->onWorkerSend(this->minute, ResourceType::schweppes);
            }
            --availableWorkers;
        } 
        //check if he takes from someone
        //according to answers from discord it isn't necessary
    }

	ActionHandlerWrapper *actionHandler = nullptr;

    int minute;
    int schweppes;
    int banana;
    int availableWorkers;
    int nextClientIndex;

    /* auto leavingClients = priority_queue<int, vector<int>>{leavesAfter}; */
    queue<ClientWrapper> arrivingClients;
    list<ClientWrapper> waitingClients;
    queue<RestockingMission> pendingRestocks;

    int getLeaveTime(const ClientWrapper &client) const {
        if (client.banana <= banana && client.schweppes <= schweppes)
            return this->minute;
        return client.arriveMinute + client.maxWaitTime;
    }

    bool leavesFirst(const ClientWrapper &clientA, const ClientWrapper &clientB) const {
        int AleaveTime = getLeaveTime(clientA);
        int BleaveTime = getLeaveTime(clientB);
        if (AleaveTime < BleaveTime) 
            return true;
        if (AleaveTime > BleaveTime) 
            return false;
        return clientA.index < clientB.index;
    }

    //consider removing const if it makes you implement const_iterator
    typename decltype(waitingClients)::const_iterator getLeavingClientIT() const {
        assert(!waitingClients.empty());
        auto returnIT = waitingClients.end();
        for (auto it = waitingClients.begin(); it != waitingClients.end(); ++it) {
            const auto &client = *it;
            assert(client.arriveMinute + client.maxWaitTime >= this->minute);
            if (returnIT == waitingClients.end()) {
                returnIT = it;
            } else if (leavesFirst(*it, *returnIT)) {
                returnIT = it;
            }
        }
        return returnIT;
    }
};

Store *createStore() {
	return new MyStore();
}
