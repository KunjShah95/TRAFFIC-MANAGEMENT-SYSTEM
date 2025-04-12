#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <unordered_map> // Added for hashing
#include <queue>         // Added for queue operations
#include <list>          // Using list as a base for queue-like processing
#include <limits>        // Required for numeric_limits

using namespace std;

// Vehicle class definition (remains the same)
class Vehicle
{
public:
    int id;
    string type;
    string category;
    time_t arrivalTime;
    int priority; // 1: Emergency, 2: Public, 0: Private (lower number = higher priority)
    int passengers;
    string route;

    Vehicle(int id, string type, string category, time_t arrivalTime, int priority = 0, int passengers = 0, string route = "")
        : id(id), type(type), category(category), arrivalTime(arrivalTime), priority(priority), passengers(passengers), route(route) {}

    void display() const
    {
        cout << "Vehicle ID: " << id << "\nType: " << type << "\nCategory: " << category << "\nArrival Time: " << ctime(&arrivalTime)
             << "Priority: " << priority << "\nPassengers: " << passengers << "\nRoute: " << route << endl;
    }
};

// Node structure for linked list (can be simplified or removed if only using std containers)
// For this example, we'll keep the linked list for display but use other structures for operations.
struct Node
{
    Vehicle *vehicle;
    Node *next;
    Node(Vehicle *v) : vehicle(v), next(nullptr) {}
};

// Enhanced Traffic Manager class
class TrafficManager
{
private:
    int capacity;
    // Using std::list for efficient insertion/deletion, acts like our primary queue
    list<Vehicle *> vehicleQueue;
    // Using std::unordered_map for fast searching by ID (Hashing)
    unordered_map<int, Vehicle *> vehicleMap;
    // Keep track of all vehicles for sorting/reporting if needed (optional redundancy)
    vector<Vehicle *> allVehiclesSnapshot;

public:
    TrafficManager(int cap) : capacity(cap) {}

    ~TrafficManager()
    {
        // Clean up allocated memory
        // Iterate through the map as it holds all unique vehicle pointers
        for (auto const &[id, vehicle_ptr] : vehicleMap)
        {
            if (vehicle_ptr != nullptr)
            { // Check if pointer is valid before deleting
                delete vehicle_ptr;
            }
        }
        // Clear containers to avoid dangling pointers after deletion
        vehicleMap.clear();
        vehicleQueue.clear();
        allVehiclesSnapshot.clear();
    }

    // Add vehicle using Queue and Hash Map
    bool addVehicle(Vehicle *v)
    {
        if (!v)
            return false; // Check for null pointer

        if (vehicleMap.size() >= capacity)
        {
            cout << "Traffic queue is full. Cannot add vehicle ID: " << v->id << endl;
            delete v; // Clean up the vehicle object if not added
            return false;
        }
        // Check if ID already exists (optional, depends on requirements)
        if (vehicleMap.count(v->id))
        {
            cout << "Error: Vehicle ID " << v->id << " already exists." << endl;
            delete v; // Clean up duplicate
            return false;
        }

        vehicleQueue.push_back(v);        // Add to the end of the queue
        vehicleMap[v->id] = v;            // Add to hash map for fast lookup
        allVehiclesSnapshot.push_back(v); // Add to snapshot vector
        cout << "Vehicle ID: " << v->id << " added to the queue." << endl;
        return true;
    }

    // Remove vehicle (demonstration, typically done during processing)
    void removeVehicle(int id)
    {
        auto it_map = vehicleMap.find(id);
        if (it_map != vehicleMap.end())
        {
            Vehicle *v_to_remove = it_map->second;
            vehicleMap.erase(it_map); // Remove from hash map

            // Remove from the queue (list) - O(n) for list, but necessary
            vehicleQueue.remove(v_to_remove);

            // Remove from snapshot vector - O(n)
            // Using erase-remove idiom
            allVehiclesSnapshot.erase(
                remove(allVehiclesSnapshot.begin(), allVehiclesSnapshot.end(), v_to_remove),
                allVehiclesSnapshot.end());

            cout << "Vehicle ID: " << id << " removed." << endl;
            delete v_to_remove; // Free memory
        }
        else
        {
            cout << "Vehicle ID: " << id << " not found for removal." << endl;
        }
    }

    // Search for vehicle by ID using Hash Map
    Vehicle *searchVehicle(int id) const
    {
        auto it = vehicleMap.find(id);
        if (it != vehicleMap.end())
        {
            return it->second; // Return pointer to the vehicle
        }
        return nullptr; // Not found
    }

    // Sort vehicles (using the snapshot vector)
    void sortVehiclesByTime()
    {
        sort(allVehiclesSnapshot.begin(), allVehiclesSnapshot.end(), [](const Vehicle *a, const Vehicle *b)
             {
                 if (!a || !b) return false; // Basic null check
                 return a->arrivalTime < b->arrivalTime; });
        cout << "Vehicle snapshot sorted by arrival time." << endl;
        // Note: This only sorts the snapshot vector, not the actual processing queue.
    }

    void sortVehiclesByPriority()
    {
        sort(allVehiclesSnapshot.begin(), allVehiclesSnapshot.end(), [](const Vehicle *a, const Vehicle *b)
             {
                 if (!a || !b)
                     return false; // Basic null check
                 if (a->priority != b->priority)
                 {
                     return a->priority < b->priority; // Lower number = higher priority
                 }
                 return a->arrivalTime < b->arrivalTime; // Secondary sort by time
             });
        cout << "Vehicle snapshot sorted by priority (then arrival time)." << endl;
    }

    // Display all vehicles currently in the queue
    void displayQueue() const
    {
        cout << "\n---- Current Traffic Queue ----\n";
        if (vehicleQueue.empty())
        {
            cout << "Queue is empty." << endl;
            return;
        }
        int count = 1;
        for (const auto *v : vehicleQueue)
        {
            if (v)
            { // Check for null pointer
                cout << "\nPosition: " << count++ << endl;
                v->display();
            }
        }
        cout << "-----------------------------\n";
    }

    // Display all vehicles from the sorted snapshot
    void displaySortedSnapshot() const
    {
        cout << "\n---- Sorted Vehicle Snapshot ----\n";
        if (allVehiclesSnapshot.empty())
        {
            cout << "No vehicles recorded." << endl;
            return;
        }
        for (const auto *v : allVehiclesSnapshot)
        {
            if (v)
            { // Check for null pointer
                v->display();
                cout << "----------" << endl;
            }
        }
        cout << "------------------------------\n";
    }

    // Process vehicles (FIFO, but prioritizing emergency)
    void processNextVehicle()
    {
        if (vehicleQueue.empty())
        {
            cout << "No vehicles to process." << endl;
            return;
        }

        Vehicle *vehicleToProcess = nullptr;
        auto it_to_erase = vehicleQueue.end(); // Iterator to the element to erase

        // Check for emergency vehicles first (priority 1)
        for (auto it = vehicleQueue.begin(); it != vehicleQueue.end(); ++it)
        {
            if (*it && (*it)->priority == 1)
            { // Check pointer and priority
                vehicleToProcess = *it;
                it_to_erase = it; // Mark this iterator for removal
                break;
            }
        }

        // If no emergency vehicle, process the one at the front (FIFO)
        if (!vehicleToProcess && !vehicleQueue.empty())
        {
            vehicleToProcess = vehicleQueue.front();
            it_to_erase = vehicleQueue.begin(); // Mark front for removal
        }

        // If a vehicle was selected for processing
        if (vehicleToProcess)
        {
            cout << "\nProcessing Vehicle ID: " << vehicleToProcess->id << endl;
            vehicleToProcess->display();

            // Clean up processed vehicle from map and snapshot
            vehicleMap.erase(vehicleToProcess->id);
            allVehiclesSnapshot.erase(
                remove(allVehiclesSnapshot.begin(), allVehiclesSnapshot.end(), vehicleToProcess),
                allVehiclesSnapshot.end());

            // Erase from the queue using the saved iterator
            if (it_to_erase != vehicleQueue.end())
            {
                vehicleQueue.erase(it_to_erase);
            }

            delete vehicleToProcess; // Free memory
            cout << "Vehicle processed and removed." << endl;
        }
        else if (!vehicleQueue.empty())
        {
            // This case might happen if the queue contains only nullptrs, though unlikely with current logic
            cout << "Error: Could not select a vehicle to process." << endl;
            // Optionally clear invalid entries here
        }
    }
};

// Main function with updated interface
int main()
{
    srand(time(0)); // Seed random number generator
    int capacity;
    cout << "Enter traffic queue capacity: ";
    while (!(cin >> capacity) || capacity <= 0)
    { // Input validation for capacity
        cout << "Invalid capacity. Please enter a positive integer: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    TrafficManager manager(capacity);
    int vehicleId = 1; // Start vehicle IDs from 1

    while (true)
    {
        cout << "\n==== Traffic Control Center ====\n"
             << "1. Add Private Vehicle (Priority 0)\n"
             << "2. Add Public Transport (Priority 2)\n"
             << "3. Add Emergency Vehicle (Priority 1)\n"
             << "4. Process Next Vehicle\n"
             << "5. View Current Queue\n"
             << "6. Search Vehicle by ID\n"
             << "7. Sort Snapshot by Time\n"
             << "8. Sort Snapshot by Priority\n"
             << "9. View Sorted Snapshot\n"
             << "10. Remove Vehicle by ID\n"
             << "11. Exit\n"
             << "Choice: ";

        int choice;
        cin >> choice;

        // Input validation for choice
        if (cin.fail())
        {
            cout << "Invalid input. Please enter a number between 1 and 11." << endl;
            cin.clear();                                         // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input
            continue;                                            // Skip the rest of the loop iteration
        }

        switch (choice)
        {
        case 1: // Add Private
        {
            string types[] = {"Car", "Truck", "Motorcycle"};
            string type = types[rand() % 3];
            manager.addVehicle(new Vehicle(
                vehicleId++, type, "Private", time(0), 0)); // Priority 0
            break;
        }
        case 2: // Add Public
        {
            string routes[] = {"Bus-101", "Bus-202", "Tram-A1", "Tram-B2"};
            string route = routes[rand() % 4];
            int passengers = 10 + rand() % 50; // Random passengers between 10 and 59
            manager.addVehicle(new Vehicle(
                vehicleId++,
                (route.find("Bus") != string::npos) ? "Bus" : "Tram", // Determine type from route
                "Public",
                time(0),
                2, // Priority 2
                passengers,
                route));
            break;
        }
        case 3: // Add Emergency
        {
            string types[] = {"Ambulance", "Police Car", "Fire Truck"};
            string type = types[rand() % 3];
            manager.addVehicle(new Vehicle(
                vehicleId++, type, "Emergency", time(0), 1)); // Priority 1
            break;
        }
        case 4: // Process
            manager.processNextVehicle();
            break;
        case 5: // View Queue
            manager.displayQueue();
            break;
        case 6: // Search
        {
            int id;
            cout << "Enter vehicle ID to search: ";
            cin >> id;
            if (cin.fail())
            { // Check if input failed (e.g., non-integer entered)
                cout << "Invalid ID format. Please enter a number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break; // Go back to the main menu
            }
            Vehicle *found = manager.searchVehicle(id);
            if (found)
            {
                cout << "Vehicle Found:\n";
                found->display();
            }
            else
            {
                cout << "Vehicle ID: " << id << " not found!" << endl;
            }
            break;
        }
        case 7: // Sort by Time
            manager.sortVehiclesByTime();
            // Optionally display after sorting:
            // manager.displaySortedSnapshot();
            break;
        case 8: // Sort by Priority
            manager.sortVehiclesByPriority();
            // Optionally display after sorting:
            // manager.displaySortedSnapshot();
            break;
        case 9: // View Sorted Snapshot
            manager.displaySortedSnapshot();
            break;
        case 10: // Remove Vehicle
        {
            int id;
            cout << "Enter vehicle ID to remove: ";
            cin >> id;
            if (cin.fail())
            { // Check if input failed
                cout << "Invalid ID format. Please enter a number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break; // Go back to the main menu
            }
            manager.removeVehicle(id);
            break;
        }
        case 11: // Exit
            cout << "Exiting Traffic Control Center." << endl;
            return 0; // Exit the program
        default:
            cout << "Invalid choice! Please enter a number between 1 and 11." << endl;
        }
    }

    // This part is technically unreachable because of the infinite loop and return 0 in case 11
    return 0;
}