/*
 * ==============================================================================
 * Project Title   : Smart Parking Allocation and Reservation Platform
 * Course          : Data Structures and Algorithms - II (CCSE0301)
 * Institution     : Noida Institute of Engineering & Technology (NIET), Greater Noida
 * Department      : Department of Computer Science & Engineering (B.Tech CS)
 * Student Name    : Isha Sharma
 * Roll No / ERP ID: 2501330120091
 * Faculty Mentor  : Ms. Pooja Sharma
 * Milestone       : PBL Final Report / Review 3: Final Review & Evaluation
 * SDG Alignment   : SDG 11 - Sustainable Cities and Communities
 * ==============================================================================
 *
 * DSA-II CONCEPTS APPLIED:
 * ------------------------------------------------------------------------------
 * Unit I   : Asymptotic Analysis & Divide-and-Conquer (Merge Sort & Binary Search)
 * Unit II  : Greedy Algorithms (Greedy Nearest-Fit Slot Allocation)
 * Unit III : Dynamic Programming (Weighted Interval Scheduling for Slot Revenue)
 * Unit IV  : Backtracking (Constrained Multi-Attribute Slot Matching)
 * Unit V   : String Matching (Knuth-Morris-Pratt [KMP] Algorithm for Plate Search)
 * ==============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SLOTS 30
#define MAX_VEHICLES 50
#define MAX_RESERVATIONS 50
#define PLATE_LEN 16
#define NAME_LEN 32

/* Enums for Parking Classification */
typedef enum {
    TYPE_TWO_WHEELER = 0,
    TYPE_COMPACT = 1,
    TYPE_SEDAN = 2,
    TYPE_SUV = 3,
    TYPE_EV = 4
} VehicleType;

typedef enum {
    SLOT_AVAILABLE = 0,
    SLOT_OCCUPIED = 1,
    SLOT_RESERVED = 2,
    SLOT_MAINTENANCE = 3
} SlotStatus;

/* Structure: Parking Slot */
typedef struct {
    int slotId;             // Unique ID (1 to MAX_SLOTS)
    int floor;              // 1, 2, or 3
    char zone;              // 'A', 'B', 'C'
    VehicleType supportedType; // Max vehicle size supported
    bool hasEVCharger;      // true if EV charging is available
    bool isHandicapAccessible; // true if near elevator/ground floor
    int distanceFromGate;  // Distance in meters from entry gate
    double hourlyRate;      // Rate in INR per hour
    SlotStatus status;      // Current status
    char occupiedByPlate[PLATE_LEN]; // License plate of occupying vehicle
    time_t entryTimestamp;  // Timestamp when vehicle parked
} ParkingSlot;

/* Structure: Vehicle Record */
typedef struct {
    char licensePlate[PLATE_LEN];
    char ownerName[NAME_LEN];
    VehicleType vehicleType;
    bool requiresEV;
    bool requiresHandicap;
    int allocatedSlotId;
    time_t checkInTime;
    time_t checkOutTime;
    double totalBilled;
} Vehicle;

/* Structure: Advance Reservation Request (for DP scheduling) */
typedef struct {
    int reservationId;
    char vehiclePlate[PLATE_LEN];
    int startHour;  // 0 to 23
    int endHour;    // 1 to 24
    int weightProfit; // Revenue / utility score
    int assignedSlotId;
    bool isScheduled;
} ReservationRequest;

/* Global System State */
ParkingSlot parkingLot[MAX_SLOTS];
int totalSlots = 20;

Vehicle vehicleRegistry[MAX_VEHICLES];
int totalVehicles = 0;

ReservationRequest reservationQueue[MAX_RESERVATIONS];
int totalReservations = 0;

/* Helper string conversion */
const char* getVehicleTypeName(VehicleType t) {
    switch (t) {
        case TYPE_TWO_WHEELER: return "Two-Wheeler";
        case TYPE_COMPACT:     return "Compact Car";
        case TYPE_SEDAN:       return "Sedan";
        case TYPE_SUV:         return "SUV";
        case TYPE_EV:          return "Electric Vehicle";
        default:               return "Unknown";
    }
}

const char* getSlotStatusName(SlotStatus s) {
    switch (s) {
        case SLOT_AVAILABLE:   return "AVAILABLE";
        case SLOT_OCCUPIED:    return "OCCUPIED";
        case SLOT_RESERVED:    return "RESERVED";
        case SLOT_MAINTENANCE: return "MAINTENANCE";
        default:               return "UNKNOWN";
    }
}

/* ==============================================================================
 * SYSTEM INITIALIZATION
 * ==============================================================================
 */
void initializeParkingLot() {
    // Initialize 20 predefined slots across 2 floors and zones A, B, C
    for (int i = 0; i < totalSlots; i++) {
        parkingLot[i].slotId = i + 1;
        parkingLot[i].floor = (i < 10) ? 1 : 2;
        parkingLot[i].zone = (i % 3 == 0) ? 'A' : ((i % 3 == 1) ? 'B' : 'C');
        
        // Distribution of slots
        if (i < 4) {
            parkingLot[i].supportedType = TYPE_TWO_WHEELER;
            parkingLot[i].hasEVCharger = false;
            parkingLot[i].isHandicapAccessible = (i == 0);
            parkingLot[i].distanceFromGate = 10 + (i * 5);
            parkingLot[i].hourlyRate = 20.0;
        } else if (i < 12) {
            parkingLot[i].supportedType = (i % 2 == 0) ? TYPE_COMPACT : TYPE_SEDAN;
            parkingLot[i].hasEVCharger = (i == 6 || i == 7);
            parkingLot[i].isHandicapAccessible = (i == 4 || i == 5);
            parkingLot[i].distanceFromGate = 25 + ((i - 4) * 8);
            parkingLot[i].hourlyRate = 40.0;
        } else if (i < 17) {
            parkingLot[i].supportedType = TYPE_SUV;
            parkingLot[i].hasEVCharger = (i == 15 || i == 16);
            parkingLot[i].isHandicapAccessible = false;
            parkingLot[i].distanceFromGate = 50 + ((i - 12) * 10);
            parkingLot[i].hourlyRate = 60.0;
        } else {
            parkingLot[i].supportedType = TYPE_EV;
            parkingLot[i].hasEVCharger = true;
            parkingLot[i].isHandicapAccessible = true;
            parkingLot[i].distanceFromGate = 35 + ((i - 17) * 12);
            parkingLot[i].hourlyRate = 50.0;
        }
        
        parkingLot[i].status = SLOT_AVAILABLE;
        strcpy(parkingLot[i].occupiedByPlate, "NONE");
        parkingLot[i].entryTimestamp = 0;
    }
}

/* ==============================================================================
 * UNIT I: DIVIDE-AND-CONQUER (MERGE SORT & BINARY SEARCH)
 * ==============================================================================
 * Application: Sorting parking slots by proximity (distance from gate) and
 * searching slots efficiently by unique slotId.
 */

// Merge helper for Merge Sort
void mergeSlotsByDistance(ParkingSlot arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    ParkingSlot* L = (ParkingSlot*)malloc(n1 * sizeof(ParkingSlot));
    ParkingSlot* R = (ParkingSlot*)malloc(n2 * sizeof(ParkingSlot));

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].distanceFromGate <= R[j].distanceFromGate) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

// Divide-and-Conquer: Merge Sort Implementation (Time: O(n log n), Space: O(n))
void mergeSortSlotsByDistance(ParkingSlot arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortSlotsByDistance(arr, left, mid);
        mergeSortSlotsByDistance(arr, mid + 1, right);
        mergeSlotsByDistance(arr, left, mid, right);
    }
}

// Binary Search for Slot by Slot ID (Time: O(log n))
int binarySearchSlotById(ParkingSlot arr[], int low, int high, int targetId) {
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid].slotId == targetId) return mid;
        if (arr[mid].slotId < targetId) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/* ==============================================================================
 * UNIT II: GREEDY ALGORITHMS (GREEDY NEAREST-FIT ALLOCATION)
 * ==============================================================================
 * Strategy: At vehicle arrival, greedily allocate the AVAILABLE slot with the
 * MINIMUM distance from the main entrance gate that supports the vehicle type.
 * Complexity: O(n) scan over available slots, O(1) space.
 */
int greedyAllocateNearestSlot(VehicleType vType, bool requiresEV, bool requiresHandicap) {
    int bestSlotIndex = -1;
    int minDistance = 99999;

    for (int i = 0; i < totalSlots; i++) {
        if (parkingLot[i].status == SLOT_AVAILABLE) {
            // Check vehicle type compatibility
            if (parkingLot[i].supportedType >= vType) {
                // Check EV charger constraint
                if (requiresEV && !parkingLot[i].hasEVCharger) continue;
                // Check handicap accessibility constraint
                if (requiresHandicap && !parkingLot[i].isHandicapAccessible) continue;

                // Greedy choice: select minimum distance
                if (parkingLot[i].distanceFromGate < minDistance) {
                    minDistance = parkingLot[i].distanceFromGate;
                    bestSlotIndex = i;
                }
            }
        }
    }
    return bestSlotIndex;
}

/* ==============================================================================
 * UNIT III: DYNAMIC PROGRAMMING (RESOURCE ALLOCATION & REVENUE OPTIMIZATION)
 * ==============================================================================
 * Problem Formulation: Weighted Interval Scheduling for Slot Reservation.
 * Given n reservation requests with [startHour, endHour] and profit/utility w_i,
 * select a subset of non-overlapping reservations to maximize total revenue.
 * Recurrence: OPT(j) = max(profit_j + OPT(p(j)), OPT(j - 1))
 * Complexity: O(n log n) sorting + O(n log n) binary search = O(n log n) time, O(n) space.
 */

// Helper comparison for sorting requests by finish time
int compareRequests(const void* a, const void* b) {
    ReservationRequest* r1 = (ReservationRequest*)a;
    ReservationRequest* r2 = (ReservationRequest*)b;
    return r1->endHour - r2->endHour;
}

// Find the latest non-conflicting reservation before index i using binary search
int findLatestNonConflicting(ReservationRequest requests[], int index) {
    int low = 0, high = index - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        if (requests[mid].endHour <= requests[index].startHour) {
            if (requests[mid + 1].endHour <= requests[index].startHour)
                low = mid + 1;
            else
                return mid;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

// Dynamic Programming: Compute Maximum Revenue Schedule
int dpOptimizeSlotRevenue(ReservationRequest requests[], int n, int selectedIndices[], int* selectedCount) {
    if (n == 0) {
        *selectedCount = 0;
        return 0;
    }

    // Step 1: Sort by finish time
    qsort(requests, n, sizeof(ReservationRequest), compareRequests);

    // Step 2: DP Table initialization
    int* dp = (int*)malloc(n * sizeof(int));
    int* p = (int*)malloc(n * sizeof(int)); // Predecessor array

    for (int i = 0; i < n; i++) {
        p[i] = findLatestNonConflicting(requests, i);
    }

    dp[0] = requests[0].weightProfit;

    // Step 3: Recurrence computation
    for (int i = 1; i < n; i++) {
        int inclProfit = requests[i].weightProfit;
        if (p[i] != -1) inclProfit += dp[p[i]];
        int exclProfit = dp[i - 1];
        dp[i] = (inclProfit > exclProfit) ? inclProfit : exclProfit;
    }

    // Step 4: Backtrack to find selected intervals
    int maxRevenue = dp[n - 1];
    int curr = n - 1;
    int count = 0;
    int tempIndices[MAX_RESERVATIONS];

    while (curr >= 0) {
        int inclProfit = requests[curr].weightProfit;
        if (p[curr] != -1) inclProfit += dp[p[curr]];

        if (curr == 0 || inclProfit >= dp[curr - 1]) {
            tempIndices[count++] = curr;
            curr = p[curr];
        } else {
            curr--;
        }
    }

    // Reverse to chronological order
    for (int i = 0; i < count; i++) {
        selectedIndices[i] = tempIndices[count - 1 - i];
    }
    *selectedCount = count;

    free(dp);
    free(p);
    return maxRevenue;
}

/* ==============================================================================
 * UNIT IV: BACKTRACKING (CONSTRAINED MULTI-ATTRIBUTE SLOT ASSIGNMENT)
 * ==============================================================================
 * Problem Formulation: Assign slots to a batch of arriving vehicles under
 * complex multi-dimensional constraints (type, floor preference, EV, handicap).
 * Backtracking explores feasible assignments and backtracks when constraints fail.
 */
bool isSlotAssignmentValid(int vehicleIdx, int slotIdx, Vehicle vList[], ParkingSlot sList[]) {
    if (sList[slotIdx].status != SLOT_AVAILABLE) return false;
    if (sList[slotIdx].supportedType < vList[vehicleIdx].vehicleType) return false;
    if (vList[vehicleIdx].requiresEV && !sList[slotIdx].hasEVCharger) return false;
    if (vList[vehicleIdx].requiresHandicap && !sList[slotIdx].isHandicapAccessible) return false;
    return true;
}

bool backtrackAssignSlots(int vIndex, int numVehicles, Vehicle vList[], int assignment[]) {
    // Base Case: All vehicles assigned
    if (vIndex == numVehicles) return true;

    // Try assigning current vehicle to every feasible slot
    for (int s = 0; s < totalSlots; s++) {
        if (isSlotAssignmentValid(vIndex, s, vList, parkingLot)) {
            // Make choice
            parkingLot[s].status = SLOT_RESERVED;
            assignment[vIndex] = parkingLot[s].slotId;

            // Recurse to next vehicle
            if (backtrackAssignSlots(vIndex + 1, numVehicles, vList, assignment)) {
                return true;
            }

            // Backtrack: Undo choice
            parkingLot[s].status = SLOT_AVAILABLE;
            assignment[vIndex] = -1;
        }
    }
    return false; // No valid assignment found along this branch
}

/* ==============================================================================
 * UNIT V: STRING MATCHING (KNUTH-MORRIS-PRATT [KMP] ALGORITHM)
 * ==============================================================================
 * Application: Searching vehicle registration plates by partial pattern
 * (e.g., city prefix "UP16", state code "DL", or numeric sequence).
 * Complexity: O(m) preprocessing + O(n) matching, avoiding quadratic O(n*m).
 */
void computeKMP_LPS(const char* pattern, int m, int lps[]) {
    int len = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

bool kmpSearch(const char* text, const char* pattern) {
    int n = strlen(text);
    int m = strlen(pattern);
    if (m == 0) return true;
    if (n < m) return false;

    int* lps = (int*)malloc(m * sizeof(int));
    computeKMP_LPS(pattern, m, lps);

    int i = 0; // index for text
    int j = 0; // index for pattern
    bool found = false;

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            found = true;
            break;
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }

    free(lps);
    return found;
}

/* ==============================================================================
 * CORE APPLICATION WORKFLOW FUNCTIONS
 * ==============================================================================
 */

void displayParkingLotStatus() {
    printf("\n========================================================================================\n");
    printf("                     SMART PARKING PLATFORM - CURRENT LOT STATUS                        \n");
    printf("========================================================================================\n");
    printf("%-5s | %-5s | %-5s | %-13s | %-7s | %-8s | %-6s | %-11s | %-10s\n",
           "Slot", "Floor", "Zone", "Max Type", "EV Chg", "Handicap", "Dist(m)", "Status", "Occupied By");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < totalSlots; i++) {
        printf("#%-4d | F-%-3d | %-5c | %-13s | %-7s | %-8s | %-6d | %-11s | %-10s\n",
               parkingLot[i].slotId,
               parkingLot[i].floor,
               parkingLot[i].zone,
               getVehicleTypeName(parkingLot[i].supportedType),
               parkingLot[i].hasEVCharger ? "YES" : "NO",
               parkingLot[i].isHandicapAccessible ? "YES" : "NO",
               parkingLot[i].distanceFromGate,
               getSlotStatusName(parkingLot[i].status),
               parkingLot[i].occupiedByPlate);
    }
    printf("========================================================================================\n\n");
}

bool registerAndParkVehicle(const char* plate, const char* owner, VehicleType vType, bool reqEV, bool reqHandicap) {
    printf("\n--> [PARK REQUEST] Processing: Plate=%s, Owner=%s, Type=%s, EV=%d, Handicap=%d\n",
           plate, owner, getVehicleTypeName(vType), reqEV, reqHandicap);

    // Apply Unit II: Greedy Nearest Allocation
    int slotIdx = greedyAllocateNearestSlot(vType, reqEV, reqHandicap);

    if (slotIdx == -1) {
        printf("    [FAIL] No suitable vacant slot found matching all constraints!\n");
        return false;
    }

    // Allocate
    parkingLot[slotIdx].status = SLOT_OCCUPIED;
    strncpy(parkingLot[slotIdx].occupiedByPlate, plate, PLATE_LEN - 1);
    parkingLot[slotIdx].entryTimestamp = time(NULL);

    // Record in Vehicle Registry
    Vehicle* v = &vehicleRegistry[totalVehicles++];
    strncpy(v->licensePlate, plate, PLATE_LEN - 1);
    strncpy(v->ownerName, owner, NAME_LEN - 1);
    v->vehicleType = vType;
    v->requiresEV = reqEV;
    v->requiresHandicap = reqHandicap;
    v->allocatedSlotId = parkingLot[slotIdx].slotId;
    v->checkInTime = parkingLot[slotIdx].entryTimestamp;
    v->totalBilled = 0.0;

    printf("    [SUCCESS] Allocated Slot #%d (Floor %d, Zone %c, Dist: %dm, Rate: INR %.1f/hr)\n",
           parkingLot[slotIdx].slotId, parkingLot[slotIdx].floor, parkingLot[slotIdx].zone,
           parkingLot[slotIdx].distanceFromGate, parkingLot[slotIdx].hourlyRate);
    return true;
}

bool releaseVehicleAndBill(const char* plate, int durationHours) {
    printf("\n--> [EXIT REQUEST] Vehicle Plate=%s, Stated Duration=%d hours\n", plate, durationHours);

    int foundSlotIdx = -1;
    for (int i = 0; i < totalSlots; i++) {
        if (parkingLot[i].status == SLOT_OCCUPIED && strcmp(parkingLot[i].occupiedByPlate, plate) == 0) {
            foundSlotIdx = i;
            break;
        }
    }

    if (foundSlotIdx == -1) {
        printf("    [ERROR] Vehicle with plate %s not found in parking records!\n", plate);
        return false;
    }

    double billAmount = parkingLot[foundSlotIdx].hourlyRate * durationHours;
    if (parkingLot[foundSlotIdx].hasEVCharger) {
        billAmount += 50.0; // Flat EV charging surcharge
    }

    // Reset slot
    parkingLot[foundSlotIdx].status = SLOT_AVAILABLE;
    strcpy(parkingLot[foundSlotIdx].occupiedByPlate, "NONE");
    parkingLot[foundSlotIdx].entryTimestamp = 0;

    printf("    [RELEASE COMPLETE] Slot #%d is now AVAILABLE.\n", parkingLot[foundSlotIdx].slotId);
    printf("    ----------------------------------------\n");
    printf("    TAX INVOICE / RECEIPT\n");
    printf("    Vehicle Plate  : %s\n", plate);
    printf("    Duration       : %d hour(s)\n", durationHours);
    printf("    Total Bill     : INR %.2f\n", billAmount);
    printf("    ----------------------------------------\n");
    return true;
}

void searchVehiclesByPlatePattern(const char* pattern) {
    printf("\n--> [KMP SEARCH] Searching for plates containing pattern: '%s'\n", pattern);
    int matchCount = 0;

    for (int i = 0; i < totalSlots; i++) {
        if (parkingLot[i].status == SLOT_OCCUPIED) {
            if (kmpSearch(parkingLot[i].occupiedByPlate, pattern)) {
                printf("    [MATCH FOUND] Slot #%d | Plate: %s | Floor: %d | Zone: %c\n",
                       parkingLot[i].slotId, parkingLot[i].occupiedByPlate,
                       parkingLot[i].floor, parkingLot[i].zone);
                matchCount++;
            }
        }
    }

    if (matchCount == 0) {
        printf("    No vehicles matched the given pattern.\n");
    } else {
        printf("    Total %d matching vehicle(s) found.\n", matchCount);
    }
}

/* ==============================================================================
 * DEMONSTRATION & TEST HARNESS
 * ==============================================================================
 */
void runFullPBLDemonstration() {
    printf("================================================================================\n");
    printf("     NOIDA INSTITUTE OF ENGINEERING AND TECHNOLOGY (NIET), GREATER NOIDA        \n");
    printf("          DEPARTMENT OF COMPUTER SCIENCE & ENGINEERING - B.TECH CS              \n");
    printf("       PBL FINAL DEMONSTRATION: SMART PARKING ALLOCATION & RESERVATION          \n");
    printf("       Student: Isha Sharma (2501330120091) | Course: DSA-II (CCSE0301)         \n");
    printf("================================================================================\n\n");

    initializeParkingLot();
    displayParkingLotStatus();

    // -------------------------------------------------------------------------
    // TEST 1: UNIT I - MERGE SORT & BINARY SEARCH
    // -------------------------------------------------------------------------
    printf("\n>>> TEST SUITE 1: UNIT I - DIVIDE & CONQUER (MERGE SORT & BINARY SEARCH)\n");
    ParkingSlot tempSlots[MAX_SLOTS];
    for (int i = 0; i < totalSlots; i++) tempSlots[i] = parkingLot[i];
    
    printf("Executing Merge Sort to rank parking slots strictly by gate proximity...\n");
    mergeSortSlotsByDistance(tempSlots, 0, totalSlots - 1);
    printf("Top 5 Closest Slots to Entry Gate:\n");
    for (int i = 0; i < 5; i++) {
        printf("  Rank %d: Slot #%d (Dist: %dm, Floor: %d, Rate: INR %.1f)\n",
               i + 1, tempSlots[i].slotId, tempSlots[i].distanceFromGate,
               tempSlots[i].floor, tempSlots[i].hourlyRate);
    }

    int targetSlot = 7;
    printf("\nExecuting Binary Search for Slot ID #%d...\n", targetSlot);
    int bIndex = binarySearchSlotById(parkingLot, 0, totalSlots - 1, targetSlot);
    if (bIndex != -1) {
        printf("  [FOUND] Slot #%d located at array index %d (Zone %c, Supported: %s)\n",
               targetSlot, bIndex, parkingLot[bIndex].zone,
               getVehicleTypeName(parkingLot[bIndex].supportedType));
    }

    // -------------------------------------------------------------------------
    // TEST 2: UNIT II - GREEDY NEAREST ALLOCATION
    // -------------------------------------------------------------------------
    printf("\n\n>>> TEST SUITE 2: UNIT II - GREEDY ALLOCATION (REAL-TIME VEHICLE INGRESS)\n");
    registerAndParkVehicle("UP16-AB-1234", "Aarav Gupta", TYPE_COMPACT, false, false);
    registerAndParkVehicle("UP16-EV-9999", "Neha Verma", TYPE_EV, true, false);
    registerAndParkVehicle("DL01-CS-5678", "Vikram Singh", TYPE_SEDAN, false, true);
    registerAndParkVehicle("HR26-XY-4321", "Pooja Malhotra", TYPE_SUV, false, false);
    registerAndParkVehicle("UP16-ZZ-7777", "Isha Sharma", TYPE_EV, true, true);

    displayParkingLotStatus();

    // -------------------------------------------------------------------------
    // TEST 3: UNIT III - DYNAMIC PROGRAMMING RESERVATION SCHEDULING
    // -------------------------------------------------------------------------
    printf("\n>>> TEST SUITE 3: UNIT III - DYNAMIC PROGRAMMING (RESOURCE OPTIMIZATION)\n");
    printf("Scenario: A single premium VIP slot has 6 overlapping advance booking requests.\n");
    printf("Objective: Select non-overlapping requests to maximize total revenue.\n");

    ReservationRequest batch[6] = {
        {101, "DL04-AA-1111", 8,  11, 150, 0, false}, // 8AM to 11AM, Rs. 150
        {102, "UP16-BB-2222", 9,  13, 220, 0, false}, // 9AM to 1PM, Rs. 220
        {103, "HR26-CC-3333", 11, 14, 180, 0, false}, // 11AM to 2PM, Rs. 180
        {104, "DL02-DD-4444", 13, 16, 200, 0, false}, // 1PM to 4PM, Rs. 200
        {105, "UP14-EE-5555", 15, 18, 170, 0, false}, // 3PM to 6PM, Rs. 170
        {106, "HR51-FF-6666", 17, 21, 260, 0, false}  // 5PM to 9PM, Rs. 260
    };

    int selectedIdx[6];
    int selectedCount = 0;
    int maxRev = dpOptimizeSlotRevenue(batch, 6, selectedIdx, &selectedCount);

    printf("\nOptimal Schedule Computed by Dynamic Programming:\n");
    printf("--------------------------------------------------------------------\n");
    printf("%-8s | %-14s | %-12s | %-10s\n", "Req ID", "Vehicle Plate", "Time Window", "Revenue (INR)");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < selectedCount; i++) {
        ReservationRequest r = batch[selectedIdx[i]];
        printf("#%-7d | %-14s | %02d:00-%02d:00   | INR %d.00\n",
               r.reservationId, r.vehiclePlate, r.startHour, r.endHour, r.weightProfit);
    }
    printf("--------------------------------------------------------------------\n");
    printf(">> Maximum Achievable Revenue: INR %d.00 (vs Naive greedy total)\n\n", maxRev);

    // -------------------------------------------------------------------------
    // TEST 4: UNIT IV - BACKTRACKING MULTI-CONSTRAINT SLOT ASSIGNMENT
    // -------------------------------------------------------------------------
    printf(">>> TEST SUITE 4: UNIT IV - BACKTRACKING (CONSTRAINED MULTI-ATTRIBUTE ASSIGNMENT)\n");
    printf("Scenario: Batch assignment of 3 vehicles requiring stringent specific constraints.\n");

    Vehicle testBatch[3] = {
        {"UP16-EV-1001", "Driver 1", TYPE_EV, true, true, -1, 0, 0, 0.0},      // EV + Handicap
        {"DL09-SV-2002", "Driver 2", TYPE_SUV, false, false, -1, 0, 0, 0.0},   // SUV
        {"HR26-EV-3003", "Driver 3", TYPE_EV, true, false, -1, 0, 0, 0.0}      // EV
    };

    int assignedSlots[3] = {-1, -1, -1};
    bool btSuccess = backtrackAssignSlots(0, 3, testBatch, assignedSlots);

    if (btSuccess) {
        printf("[BACKTRACKING SUCCESS] Global constraint-satisfaction assignment found:\n");
        for (int i = 0; i < 3; i++) {
            printf("  Vehicle %s (%s, EV=%d, Handicap=%d) --> Assigned Slot #%d\n",
                   testBatch[i].licensePlate, getVehicleTypeName(testBatch[i].vehicleType),
                   testBatch[i].requiresEV, testBatch[i].requiresHandicap, assignedSlots[i]);
        }
    } else {
        printf("[BACKTRACKING FAILED] No assignment satisfies all mutual constraints.\n");
    }

    // -------------------------------------------------------------------------
    // TEST 5: UNIT V - KMP STRING SEARCH FOR VEHICLE PLATES
    // -------------------------------------------------------------------------
    printf("\n>>> TEST SUITE 5: UNIT V - STRING MATCHING (KMP ALGORITHM FOR PLATE SEARCH)\n");
    searchVehiclesByPlatePattern("UP16");
    searchVehiclesByPlatePattern("EV");
    searchVehiclesByPlatePattern("DL");

    // -------------------------------------------------------------------------
    // TEST 6: VEHICLE CHECKOUT & BILLING
    // -------------------------------------------------------------------------
    printf("\n>>> TEST SUITE 6: VEHICLE DEPARTURE & BILLING SYSTEM\n");
    releaseVehicleAndBill("UP16-AB-1234", 3);
    releaseVehicleAndBill("UP16-EV-9999", 4);

    displayParkingLotStatus();

    printf("================================================================================\n");
    printf("            ALL REVIEW 3 VERIFICATION & DEMONSTRATIONS COMPLETED!              \n");
    printf("================================================================================\n");
}

int main() {
    runFullPBLDemonstration();
    return 0;
}

