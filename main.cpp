
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// ---------------- DATA STRUCTURES ----------------

struct Parking {
    int id;
    int location;
    int price;
};

struct Reservation {
    int bookingId;
    string driver;
    int parkingId;
    int startTime;
    int endTime;
    bool active;
};

struct Edge {
    int u, v, weight;
};

vector<Parking> parks = {
    {101, 1, 50},
    {102, 2, 40},
    {103, 3, 60},
    {104, 4, 30},
    {105, 2, 45}
};

vector<Reservation> bookings;

int nextBookingId = 1001;

// Road distance graph
int graph[5][5] = {
    {0, 4, 2, 0, 0},
    {4, 0, 1, 5, 0},
    {2, 1, 0, 8, 10},
    {0, 5, 8, 0, 2},
    {0, 0, 10, 2, 0}
};

// ---------------- MERGE SORT ----------------

// Sort parking by ID or price
void mergeSort(vector<Parking>& p, int l, int r, bool byPrice) {
    if (l >= r)
        return;

    int mid = (l + r) / 2;

    mergeSort(p, l, mid, byPrice);
    mergeSort(p, mid + 1, r, byPrice);

    vector<Parking> temp;
    int i = l, j = mid + 1;

    while (i <= mid && j <= r) {
        bool condition;

        if (byPrice)
            condition = p[i].price <= p[j].price;
        else
            condition = p[i].id <= p[j].id;

        if (condition)
            temp.push_back(p[i++]);
        else
            temp.push_back(p[j++]);
    }

    while (i <= mid)
        temp.push_back(p[i++]);

    while (j <= r)
        temp.push_back(p[j++]);

    for (int k = 0; k < temp.size(); k++)
        p[l + k] = temp[k];
}

// ---------------- BINARY SEARCH ----------------

int binarySearch(int id) {
    int low = 0;
    int high = parks.size() - 1;

    while (low <= high) {
        int mid = (low + high) / 2;

        if (parks[mid].id == id)
            return mid;

        if (parks[mid].id < id)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return -1;
}

// ---------------- DISPLAY PARKING ----------------

void displayParking() {
    cout << "\nID\tLocation\tPrice\n";

    for (int i = 0; i < parks.size(); i++) {
        cout << parks[i].id << "\t"
             << parks[i].location << "\t\t"
             << parks[i].price << endl;
    }
}

// ---------------- ADD PARKING ----------------

void addParking() {
    Parking p;

    cout << "Enter new parking ID: ";
    cin >> p.id;

    if (binarySearch(p.id) != -1) {
        cout << "Parking ID already exists!\n";
        return;
    }

    cout << "Enter location (0-4): ";
    cin >> p.location;

    cout << "Enter parking price: ";
    cin >> p.price;

    if (p.location < 0 || p.location >= 5 || p.price < 0) {
        cout << "Invalid location or price!\n";
        return;
    }

    parks.push_back(p);

    mergeSort(parks, 0, parks.size() - 1, false);

    cout << "Parking space added successfully!\n";
}

// ---------------- DIJKSTRA ----------------

void dijkstra(int source, int dist[]) {
    bool visited[5] = {false};

    for (int i = 0; i < 5; i++)
        dist[i] = INT_MAX;

    dist[source] = 0;

    for (int count = 0; count < 5; count++) {
        int u = -1;

        for (int i = 0; i < 5; i++) {
            if (!visited[i] &&
                (u == -1 || dist[i] < dist[u]))
                u = i;
        }

        if (u == -1 || dist[u] == INT_MAX)
            break;

        visited[u] = true;

        for (int v = 0; v < 5; v++) {
            if (graph[u][v] > 0 && !visited[v] &&
                dist[u] + graph[u][v] < dist[v]) {

                dist[v] = dist[u] + graph[u][v];
            }
        }
    }
}

// ---------------- RESERVATION CHECK ----------------

// Check whether two time slots overlap
bool hasConflict(int parkingId, int start, int end) {
    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].parkingId == parkingId &&
            bookings[i].active) {

            if (start < bookings[i].endTime &&
                end > bookings[i].startTime)
                return true;
        }
    }

    return false;
}

// ---------------- GREEDY ALLOCATION ----------------

void bookParking() {
    string name;
    int source, start, end;

    cout << "Enter driver name (one word): ";
    cin >> name;

    cout << "Enter your location (0-4): ";
    cin >> source;

    if (source < 0 || source >= 5) {
        cout << "Invalid location!\n";
        return;
    }

    cout << "Enter starting hour: ";
    cin >> start;

    cout << "Enter ending hour: ";
    cin >> end;

    if (start < 0 || end > 24 || start >= end) {
        cout << "Invalid time slot!\n";
        return;
    }

    int dist[5];
    dijkstra(source, dist);

    int best = -1;

    // Greedy: choose nearest available parking
    for (int i = 0; i < parks.size(); i++) {
        int id = parks[i].id;
        int loc = parks[i].location;

        if (dist[loc] == INT_MAX)
            continue;

        if (!hasConflict(id, start, end)) {
            if (best == -1 ||
                dist[loc] < dist[parks[best].location]) {

                best = i;
            }
        }
    }

    if (best == -1) {
        cout << "Sorry! No parking available for this time.\n";
        return;
    }

    Reservation r;

    r.bookingId = nextBookingId++;
    r.driver = name;
    r.parkingId = parks[best].id;
    r.startTime = start;
    r.endTime = end;
    r.active = true;

    bookings.push_back(r);

    cout << "\n===== BOOKING CONFIRMED =====\n";
    cout << "Booking ID: " << r.bookingId << endl;
    cout << "Driver: " << name << endl;
    cout << "Parking ID: " << r.parkingId << endl;
    cout << "Distance: "
         << dist[parks[best].location] << " km\n";
    cout << "Price: Rs. " << parks[best].price << endl;
    cout << "Time: " << start << ":00 to "
         << end << ":00\n";
}

// ---------------- CANCEL RESERVATION ----------------

void cancelBooking() {
    int id;

    cout << "Enter booking ID: ";
    cin >> id;

    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].bookingId == id &&
            bookings[i].active) {

            bookings[i].active = false;

            cout << "Reservation cancelled successfully!\n";
            return;
        }
    }

    cout << "Active reservation not found!\n";
}

// ---------------- VIEW RESERVATIONS ----------------

void viewBookings() {
    if (bookings.empty()) {
        cout << "No reservations found!\n";
        return;
    }

    cout << "\nBooking ID\tDriver\tParking\tTime\tStatus\n";

    for (int i = 0; i < bookings.size(); i++) {
        cout << bookings[i].bookingId << "\t\t"
             << bookings[i].driver << "\t"
             << bookings[i].parkingId << "\t"
             << bookings[i].startTime << "-"
             << bookings[i].endTime << "\t"
             << (bookings[i].active ? "Active" : "Cancelled")
             << endl;
    }
}

// ---------------- ACTIVITY SELECTION ----------------

void activitySelection() {
    int id;

    cout << "Enter parking ID: ";
    cin >> id;

    vector<Reservation> temp;

    for (int i = 0; i < bookings.size(); i++) {
        if (bookings[i].parkingId == id &&
            bookings[i].active) {
            temp.push_back(bookings[i]);
        }
    }

    if (temp.empty()) {
        cout << "No reservations found!\n";
        return;
    }

    // Sort reservations by ending time
    sort(temp.begin(), temp.end(),
         [](Reservation a, Reservation b) {
             return a.endTime < b.endTime;
         });

    int lastEnd = -1;
    int count = 0;

    cout << "\nCompatible reservations:\n";

    for (int i = 0; i < temp.size(); i++) {
        if (temp[i].startTime >= lastEnd) {
            cout << temp[i].bookingId << " : "
                 << temp[i].startTime << "-"
                 << temp[i].endTime << endl;

            lastEnd = temp[i].endTime;
            count++;
        }
    }

    cout << "Maximum compatible reservations: "
         << count << endl;
}

// ---------------- OCCUPANCY REPORT ----------------

void occupancyReport() {
    int hour;

    cout << "Enter hour to check (0-23): ";
    cin >> hour;

    if (hour < 0 || hour > 23) {
        cout << "Invalid hour!\n";
        return;
    }

    int occupied = 0;

    for (int i = 0; i < parks.size(); i++) {
        bool found = false;

        for (int j = 0; j < bookings.size(); j++) {
            if (bookings[j].parkingId == parks[i].id &&
                bookings[j].active &&
                hour >= bookings[j].startTime &&
                hour < bookings[j].endTime) {

                found = true;
                break;
            }
        }

        if (found)
            occupied++;
    }

    cout << "\nTotal parking spaces: "
         << parks.size() << endl;

    cout << "Occupied spaces: " << occupied << endl;

    cout << "Available spaces: "
         << parks.size() - occupied << endl;
}

// ---------------- KRUSKAL ALGORITHM ----------------

int parent[5];

int findRoot(int x) {
    if (parent[x] == x)
        return x;

    return parent[x] = findRoot(parent[x]);
}

void kruskal() {
    vector<Edge> edges = {
        {0, 1, 4},
        {0, 2, 2},
        {1, 2, 1},
        {1, 3, 5},
        {2, 3, 8},
        {2, 4, 10},
        {3, 4, 2}
    };

    sort(edges.begin(), edges.end(),
         [](Edge a, Edge b) {
             return a.weight < b.weight;
         });

    for (int i = 0; i < 5; i++)
        parent[i] = i;

    int total = 0;
    int count = 0;

    cout << "\nMinimum Cost Road Network:\n";

    for (int i = 0; i < edges.size(); i++) {
        int a = findRoot(edges[i].u);
        int b = findRoot(edges[i].v);

        if (a != b) {
            parent[a] = b;

            cout << edges[i].u << " - "
                 << edges[i].v << " : "
                 << edges[i].weight << endl;

            total += edges[i].weight;
            count++;

            if (count == 4)
                break;
        }
    }

    cout << "Total minimum cost: " << total << endl;
}

// ---------------- MAIN FUNCTION ----------------

int main() {
    int choice;

    do {
        cout << "\n================================\n";
        cout << " SMART PARKING ALLOCATION SYSTEM\n";
        cout << "================================\n";

        cout << "1. Display Parking Spaces\n";
        cout << "2. Add Parking Space\n";
        cout << "3. Sort Parking by Price\n";
        cout << "4. Search Parking by ID\n";
        cout << "5. Find Shortest Distances\n";
        cout << "6. Book Nearest Parking\n";
        cout << "7. Cancel Reservation\n";
        cout << "8. View All Reservations\n";
        cout << "9. Activity Selection\n";
        cout << "10. Occupancy Report\n";
        cout << "11. Minimum Road Network (Kruskal)\n";
        cout << "12. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayParking();
                break;

            case 2:
                addParking();
                break;

            case 3: {
                vector<Parking> temp = parks;

                mergeSort(temp, 0, temp.size() - 1, true);

                cout << "\nParking sorted by price:\n";
                for (int i = 0; i < temp.size(); i++) {
                    cout << temp[i].id << " - Rs. "
                         << temp[i].price << endl;
                }
                break;
            }

            case 4: {
                int id;
                cout << "Enter parking ID: ";
                cin >> id;

                int result = binarySearch(id);

                if (result == -1)
                    cout << "Parking not found!\n";
                else
                    cout << "Parking found! Price: Rs. "
                         << parks[result].price << endl;

                break;
            }

            case 5: {
                int source, dist[5];

                cout << "Enter your location (0-4): ";
                cin >> source;

                if (source < 0 || source >= 5) {
                    cout << "Invalid location!\n";
                    break;
                }

                dijkstra(source, dist);

                for (int i = 0; i < 5; i++) {
                    cout << "Location " << i << ": ";

                    if (dist[i] == INT_MAX)
                        cout << "Unreachable\n";
                    else
                        cout << dist[i] << " km\n";
                }

                break;
            }

            case 6:
                bookParking();
                break;

            case 7:
                cancelBooking();
                break;

            case 8:
                viewBookings();
                break;

            case 9:
                activitySelection();
                break;

            case 10:
                occupancyReport();
                break;

            case 11:
                kruskal();
                break;

            case 12:
                cout << "Thank you for using Smart Parking!\n";
                break;

            default:
                cout << "Invalid choice!\n";
        }

    } while (choice != 12);

    return 0;
}
