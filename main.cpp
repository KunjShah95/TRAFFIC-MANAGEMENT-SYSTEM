#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
using namespace std;

// Vehicle class to represent each vehicle
class Vehicle {
public:
    int id;
    string mode;        // "Private", "Bus", "Tram"
    string type;        // "Car", "Truck", "Motorcycle", "Bus", "Tram"
    int arrivalTime;
    int priority;       // 0 = Normal, 1 = Emergency, 2 = Public Transport
    int passengers;     // Number of passengers
    string route;       // For public transport (e.g., "Bus-12", "Tram-A")

    Vehicle(int id, string mode, string type, int arrivalTime, 
            int priority = 0, int passengers = 1, string route = "N/A") {
        this->id = id;
        this->mode = mode;
        this->type = type;
        this->arrivalTime = arrivalTime;
        this->priority = priority;
        this->passengers = passengers;
        this->route = route;
    }

    void display() const {
        cout << "ID: " << id 
             << " | Mode: " << mode 
             << " | Type: " << type 
             << " | Route: " << route
             << " | Passengers: " << passengers
             << " | Arrival: " << arrivalTime 
             << " | Priority: " << priority << endl;
    }

    bool isPublicTransport() const {
        return (mode == "Bus" || mode == "Tram");
    }
};

// Circular Queue to manage vehicles
class CircularQueue {
private:
    int front, rear, size, capacity;
    Vehicle** vehicles;

public:
    CircularQueue(int cap) : capacity(cap), front(-1), rear(-1), size(0) {
        vehicles = new Vehicle*[capacity];
    }

    ~CircularQueue() {
        delete[] vehicles;
    }

    bool isFull() const { return size == capacity; }
    bool isEmpty() const { return size == 0; }

    void enqueue(Vehicle* v) {
        if (isFull()) {
            cout << "Queue full! Traffic jam detected!" << endl;
            return;
        }
        if (isEmpty()) front = 0;
        rear = (rear + 1) % capacity;
        vehicles[rear] = v;
        size++;
    }

    Vehicle* dequeue() {
        if (isEmpty()) return nullptr;

        // Priority 1: Emergency vehicles
        for (int i = front; i != rear; i = (i+1)%capacity) {
            if (vehicles[i]->priority == 1) {
                Vehicle* emergency = vehicles[i];
                // Shift remaining vehicles
                for (int j = i; j != rear; j = (j+1)%capacity)
                    vehicles[j] = vehicles[(j+1)%capacity];
                rear = (rear - 1 + capacity) % capacity;
                size--;
                return emergency;
            }
        }

        // Priority 2: Public transport
        Vehicle* frontVehicle = vehicles[front];
        front = (front + 1) % capacity;
        size--;
        return frontVehicle;
    }

    void display() const {
        if (isEmpty()) {
            cout << "Queue empty" << endl;
            return;
        }
        int i = front;
        do {
            vehicles[i]->display();
            i = (i + 1) % capacity;
        } while (i != (rear + 1) % capacity);
    }
};

// Traffic System to manage the simulation
class TrafficSystem {
private:
    CircularQueue queue;
    int simulationTime;
    int totalPrivateVehicles;
    int totalPublicVehicles;
    int totalPassengers;
    vector<string> reportData;

public:
    TrafficSystem(int cap) : queue(cap), simulationTime(0), 
                            totalPrivateVehicles(0), totalPublicVehicles(0),
                            totalPassengers(0) {}

    void addVehicle(Vehicle* v) {
        queue.enqueue(v);
        if (v->isPublicTransport()) {
            totalPublicVehicles++;
            totalPassengers += v->passengers;
        } else {
            totalPrivateVehicles++;
        }
        reportData.push_back("Time " + to_string(simulationTime) + 
                            ": Added " + v->mode + " - " + v->route);
    }

    void processVehicles() {
        if (!queue.isEmpty()) {
            Vehicle* v = queue.dequeue();
            reportData.push_back("Time " + to_string(simulationTime) + 
                               ": Processed " + v->mode + " - " + v->route);
            delete v;
        }
        simulationTime++;
    }

    void generateReport(const string& filename) {
        ofstream reportFile(filename);
        reportFile << "=== Traffic Simulation Report ===\n";
        reportFile << "Simulation Duration: " << simulationTime << " units\n";
        reportFile << "Total Vehicles Processed: " 
                  << (totalPrivateVehicles + totalPublicVehicles) << "\n";
        reportFile << " - Private Vehicles: " << totalPrivateVehicles << "\n";
        reportFile << " - Public Transport: " << totalPublicVehicles << "\n";
        reportFile << "Total Passengers Carried: " << totalPassengers << "\n\n";
        reportFile << "Event Log:\n";
        for (const auto& entry : reportData) {
            reportFile << entry << "\n";
        }
        reportFile.close();
        cout << "Report generated: " << filename << endl;
    }

    void displayQueue() const {
        queue.display();
    }

    int getSimulationTime() const {
        return simulationTime;
    }
};

// Main function with menu-driven interface
int main() {
    srand(time(0));
    int capacity;
    cout << "Enter traffic queue capacity: ";
    cin >> capacity;
    
    TrafficSystem system(capacity);
    int vehicleId = 1;

    while (true) {
        cout << "\n==== Traffic Control Center ====\n"
             << "1. Add Private Vehicle\n"
             << "2. Add Public Transport\n"
             << "3. Process Vehicles\n"
             << "4. View Queue\n"
             << "5. Generate Report\n"
             << "6. Exit\n"
             << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                string types[] = {"Car", "Truck", "Motorcycle"};
                string type = types[rand() % 3];
                system.addVehicle(new Vehicle(
                    vehicleId++, "Private", type, system.getSimulationTime()
                ));
                break;
            }
            case 2: {
                string routes[] = {"Bus-101", "Bus-202", "Tram-A1", "Tram-B2"};
                string route = routes[rand() % 4];
                int passengers = 10 + rand() % 50; // 10-59 passengers
                system.addVehicle(new Vehicle(
                    vehicleId++, 
                    (route.find("Bus") != string::npos) ? "Bus" : "Tram",
                    "Public",
                    system.getSimulationTime(),
                    2,  // Priority for public transport
                    passengers,
                    route
                ));
                break;
            }
            case 3:
                system.processVehicles();
                break;
            case 4:
                system.displayQueue();
                break;
            case 5:
                system.generateReport("traffic_report.txt");
                break;
            case 6:
                system.generateReport("final_report.txt");
                return 0;
            default:
                cout << "Invalid choice!" << endl;
        }
    }
}