#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <vector>

#include "interface.h"

using std::cin;
using std::cout;
using std::endl;
using std::unique_ptr;
using std::string;
using std::vector;

string resourceToString(ResourceType resource) {
    return resource == ResourceType::banana? "banana": "schweppes";
}

struct EchoHandler : ActionHandler {
    void onWorkerSend(int minute, ResourceType resource) override {
        assert(resource == ResourceType::banana || resource == ResourceType::schweppes);
        cout << "W " << minute << ' ' << resourceToString(resource) << endl;
    }
    void onWorkerBack(int minute, ResourceType resource) override {
        assert(resource == ResourceType::banana || resource == ResourceType::schweppes);
        cout << "D " << minute << ' ' << resourceToString(resource) << endl;
    }
    void onClientDepart(int index, int minute, int banana, int schweppes) override {
        cout << index << ' ' << minute << ' ' << banana << ' ' << schweppes << endl;
    }
};

int main() {
    int W, C;
    cin >> W >> C;
    unique_ptr<Store> store(createStore());
    store->init(W, 0, 0);
    EchoHandler handler;
    store->setActionHandler(&handler);
    vector<Client> clients;
    int maxtime = 0;
    for (int i=0; i<C; ++i) {
        int arriveMinute, banana, schweppes, maxWaitTime;
        cin >> arriveMinute >> banana >> schweppes >> maxWaitTime;
        maxtime = std::max(maxtime, arriveMinute + maxWaitTime);
        clients.push_back({arriveMinute, banana, schweppes, maxWaitTime});
    }
    store->addClients(clients.data(), clients.size());
    store->advanceTo(maxtime);
    /* int q; */
    /* cin >> q; */
    /* for (int i=0; i<q; ++i) { */
    /*     int min; */
    /*     cin >> min; */
    /*     cout << "advancing to " << min << endl; */
    /*     store->advanceTo(min); */
    /* } */
}
