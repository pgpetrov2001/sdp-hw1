#include <cassert>
#include <cstdint>
#include <vector>

#include "interface.h"

using std::size_t;
using std::vector;

/// This is sample empty implementation you can place your solution here or delete this and include tests to your solution

// ???? no client will be added with an arrival time earlier than those clients that are already added
// ???? clients that can take what they want will leave in order of arrival??
// ???? first clients that can take what they want leave or clients that have waited their limit
// !!!! clients may be added before the time has been advanced to their arrival time

struct RestockingMission {
    int ETA_minute;
    ResourceType resourceType;
    int leftoverQuantity;
    //the restock quantity itself is always the same
    //however we need to know how much from the restock quantity will be left over for 
    //other clients that are yet to arrive that haven't triggered the restock
};

struct ClientWrapper {
    int index;
    Client data;
};

class MyStore : Store {
public:
	void setActionHandler(ActionHandler *handler) override {
		actionHandler = handler;
	}

	void init(int workerCount, int startBanana, int startSchweppes) override {
        minute = 0;
        availableWorkers = workerCount;
        banana = startBanana;
        schweppes = startSchweppes;
	}

    void addClient(const Client &client) {
        stack<RestockingMission> willArriveOnTime;
        //TODO: check <=
        //we iterate over all pending restocks that will arrive before the client leaves
        //and calculate how much stock will be left from those restocks for the new client
        int leftoverSchweppes = 0;
        int leftoverBanana = 0;
        while (!pendingRestocks.empty() && pendingRestocks.front().ETA_minute <= client.arriveMinute + client.maxWaitTime) { 
            if (pendingRestocks.front().resourceType == ResourceType::schweppes) {
                leftoverSchweppes += pendingRestocks.front().leftoverQuantity;
            } else {
                leftoverBanana += pendingRestocks.front().leftoverQuantity;
            }
            willArriveOnTime.push(pendingRestocks.front());
            pendingRestocks.pop();
        }
        while (!willArriveOnTime.empty()) {
            pendingRestocks.push_front(willArriveOnTime.top());
            willArriveOnTime.pop();
        }
        int schweppesShortage = client.schweppes - leftoverSchweppes - schweppes;
        int bananaShortage = client.banana - leftoverBanana - banana;
        while (workerCount && (schweppesShortage > 0 || bananaShortage > 0)) {
            if (schweppesShortage > bananaShortage) {
                pendingRestocks.push_back({
                    minute + 60,
                    ResourceType::schweppes,
                    100-schweppesShortage,
                });
                actionHandler->onWorkerSend(client.arriveMinute, ResourceType::schweppes);
                --workerCount;
                schweppesShortage -= 100;
            } else {
                pendingRestocks.push_back({
                    minute + 60,
                    ResourceType::banana,
                    100-bananaShortage,
                });
                actionHandler->onWorkerSend(client.arriveMinute, ResourceType::banana);
                --workerCount;
                bananaShortage -= 100;
            }
        }
    }

	void addClients(const Client *clients, int count) override {
        for (size_t i=0; i<count; ++i) {
            arrivingClients.push(clients[i]);
            if (i > 0) {
                assert(clients[i-1].arriveMinute <= clients[i].arriveMinute);
            }
        }
	}

	void advanceTo(int minute) override {
        this->minute = minute; // ??? is it even needed?
        //first the clients arrive
        /* while (!arrivingClients.empty() && arrivingClients.front().arriveMinute <= minute) */
        /* addClient(clients[i]); */
        //all at the same moment:
        //first arrive the restockings
        while (minute >= pendingRestocks.front().ETA_minute) {
            auto restock = pendingRestocks.front();
            if (restock.resourceType == ResourceType::schweppes) {
                schweppes += 100;
            } else {
                banana += 100;
            }
            pendingRestocks.pop_front();
        }
        //then the clients get serviced
        {
            auto it = waitingClients.begin();
            while (it != waitingClients.end()) {
                const auto [index, client] = *it;
                if (client.banana <= banana && client.schweppes <= schweppes) {
                    banana -= client.banana;
                    schweppes -= client.schweppes;
                    actionHandler->onClientDepart(index, minute, client.banana, clients.schweppes);
                    it = waitingClients.erase(it);
                } else {
                    ++it;
                }
            }
        }
        //then the clients that cannot wait anymore leave
        {
            priority_queue<ClientWrapper> leavingClients;
            auto it = waitingClients.begin();
            while (it != waitingClients.end()) {
                const auto [index, client] = *it;
                if (client.arriveMinute + client.maxWaitTime <= minute) {
                    leavingClients.push({index, client});
                    it = waitingClients.erase(it);
                } else {
                    ++it;
                }
            }

            while (!leavingClients.empty()) {
                const auto [index, client] = leavingClients.top();
                actionHandler->onClientDepart(
                    index,
                    client.arriveMinute + client.maxWaitTime,
                    std::min(client.banana, banana),
                    std::min(client.schweppes, schweppes)
                );
                banana    -= std::min(client.banana, banana);
                schweppes -= std::min(client.schweppes, schweppes);
                leavingClients.pop();
            }
        }
	}

	virtual int getBanana() const {
		return banana;
	}

	virtual int getSchweppes() const {
		return schweppes;
	}

private:
	ActionHandler *actionHandler = nullptr;

    int schweppes;
    int banana;
    int minute;
    int availableWorkers;
    queue<ClientWrapper> arrivingClients;
    list<ClientWrapper> waitingClients;
    deque<RestockingMission> pendingRestocks;
};

Store *createStore() {
	return new MyStore();
}
