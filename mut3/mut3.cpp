#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic> 

using namespace std;

atomic<int> clientCount = 0;
int maxClients = 10;


mutex clientMutex;

condition_variable clientCV;

void clientThread() {
    while (true) {

        if (clientCount.fetch_add(1) >= maxClients) {
            cout << "Очередь переполнена. Новый клиент отказывается." << endl;
            clientCount.fetch_sub(1);
            break;
        }

        cout << "Клиент добавлен в очередь. Всего клиентов: " << clientCount << endl;


        clientCV.notify_one();


        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void operatorThread() {
    while (true) {

        unique_lock<mutex> lock(clientMutex);


        clientCV.wait(lock, [] { return clientCount > 0; });
        clientCount.fetch_sub(1);

        cout << "Клиент обслуживается. Осталось клиентов: " << clientCount << endl;

        this_thread::sleep_for(chrono::milliseconds(2000));
        lock.unlock();
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "Введите максимальное количество клиентов: ";
    cin >> maxClients;

    thread one(clientThread);
    thread two(operatorThread);

    one.join();
    two.join();

    return 0;
}

