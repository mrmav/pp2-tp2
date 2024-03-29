/*
	<license>
	MIT License
	Copyright(c) 2018 Jorge Noro
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files(the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions :
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
	</license>
	<author> Jorge Noro </author>
	<version>1.0.0</version>
	<summary>User functions implementation</summary>
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Types.h"
#include "Hashtable.h"
#include "CreateFunctions.h"
#include "UserFunctions.h"
#include "Utils.h"
#include "InfoOutput.h"

/*
Lets the user create a new flight
*/
Flight* CreateNewFlight(HashTable* ht, Airplane* airplane) {

	if (ht == NULL || airplane == NULL) {

		return NULL;

	}

	Flight* f = NULL;
	char buffer[20];
	struct tm* departureDate;

	// get data from the user

	printf("### Create Flight: ###\n");

	printf("- Input flight date in the format yyyy/mm/dd hh:mm :\n");

	fgets(buffer, sizeof(buffer), stdin);
	ClearInput();

	departureDate = NewDateFromString(buffer);

	f = CreateFlight(

		0,
		departureDate,
		NULL,
		airplane->SeatsNumber,
		CloneSeatsFromAirplane(airplane),
		airplane->ShuttleName

	);

	printf("- Is this flight priority? (0: No, 1: Yes)\n");

	int p;
	scanf("%i", &p);
	ClearInput();

	SetFlightPriority(f, p);

	// only add the flight if it is unique
	if (CheckHashTableForFlightID(ht, f->ID->value) == NULL) {

		AddNodeToHashTable(ht, CreateNode(f->ID->ddd, f));

		ListSortAscending(ht, ht->table[HashFunction(f->ID->ddd)]);
	}
	else {

		free(f);
		f = NULL;

	}

	return f;

}

/*
Check for flight empty seats
*/
int CheckFlightForSeats(HashTable* ht, long int id) {

	if (ht == NULL) {

		return -1;

	}
	
	Flight* flight = CheckHashTableForFlightID(ht, id);

	if (flight != NULL) {

		//flight exists, lets check for available seats

		printf("\nAvailable Seats: ");

		int nAvailableSeats = 0;

		for (unsigned int i = 0; i < flight->SeatsNumber; i++) {

			if (flight->Seats[i]->IsOccupied == 0) {

				printf("%02i ", i+1);

				nAvailableSeats++;

			}

		}

		printf("\nNumber of available seats: %i\n", nAvailableSeats);

		return 0;

	}
	else {

		return -2;

	}

}

/*
Lets the user book a seat
*/
int BookFlightSeat(HashTable* ht, long int id, unsigned int seatPosition) {

	if (ht == NULL) {

		return -1;

	}
	
	Flight* flight = CheckHashTableForFlightID(ht, id);
	
	if (!(seatPosition > 0 && seatPosition <= flight->SeatsNumber)) {

		return -3;

	}

	if (flight != NULL) {
	
		if (flight->Seats[seatPosition - 1]->IsOccupied == 0) {

			printf("- Seat is available:\n");
			
			char buffer[100];
			RequestName(buffer, sizeof(buffer));

			SetPassengerSeat(flight->Seats[seatPosition - 1], buffer);

			printf("- Is this seat priority? (0: No, 1: Yes)\n");

			int p;
			scanf("%i", &p);
			ClearInput();

			SetSeatPriority(flight->Seats[seatPosition - 1], p);
			
			printf("Sucessfuly booked seat!\n");

			return 0;

		}

	}

	return -2;

}

/*
Lets the user cancel a reservation
*/
int CancelBooking(HashTable* ht, long int id) {

	if (ht == NULL) {

		return -1;

	}

	Flight* flight = CheckHashTableForFlightID(ht, id);

	if (flight != NULL) {

		printf("- Please input the passenger name:\n");

		char buffer[100];
		RequestName(buffer, sizeof(buffer));

		for (int i = 0; i < flight->SeatsNumber; i++) {

			if (strcmp(buffer, flight->Seats[i]->PassengerName) == 0) {

				FreePassengerSeat(flight->Seats[i]);

				return 0;

			}

		}

		return -3;

	}

	return -2;

}

/*
Count free seats in flight
*/
int CountFreeSeats(Flight* f) {

	int nAvailableSeats = 0;

	for (unsigned int i = 0; i < f->SeatsNumber; i++) {

		if (f->Seats[i]->IsOccupied == 0) {

			nAvailableSeats++;
			
		}

	}

	return nAvailableSeats;

}

/*
Set a seat to a passenger
*/
Seat* SetPassengerSeat(Seat* seat, char name[]) {

	seat->IsOccupied = 1;
	strcpy(seat->PassengerName, name);

}

/*
Set a seat free
*/
Seat* FreePassengerSeat(Seat* seat) {

	seat->IsOccupied = 0;
	seat->IsPriority = 0;
	strcpy(seat->PassengerName, "");

}

/*
Request flight id to user
*/
long int RequestFlightID() {

	printf("- Input flight ID:\n");

	long int id;
	scanf("%ld", &id);
	ClearInput();

	return id;
}

/*
request name to user
*/
void RequestName(char* buffer, size_t size) {

	printf("- Input passenger name:\n");
	fgets(buffer, size, stdin);
	ClearInput();

	buffer[strcspn(buffer, "\n")] = 0;  // takes the new line out, and replaces it with a terminator

}

/*
get closest flight to hold n passengers
*/
Flight* GetClosestFlightHoldPassengers(HashTable* ht, int n, Airplane* plane) {

	if (ht == NULL) {

		return NULL;

	}
	
	if (n > plane->SeatsNumber) {

		return NULL;

	}

	int currentColumnToCheck = 0;

	Node* previous = NULL;

	while (1) {
		// since we have no idea when we will finish
		// trasversing the hashtable, we set an infinite loop
		// and exit mannualy.
		// the exit is guaranteed because after we find a null
		// position that can hold a flight, we will create it
		// and thus, breaking the loop.
		
		for (int i = 0; i < HASH_MAX_SIZE; i++) {
			// from top to bottom, we trasverse collumn by collumn

			Node* head = ht->table[i];

			for (int j = 0; j < currentColumnToCheck; j++) {
				// jump to desired column

				head = head->next;
			}

			if (head == NULL) {
				// if head is null, it means that we have
				// a space in hand to create a new flight
				// that is still not created.

				int year  = previous == NULL ? FIRST_YEAR  : previous->flightData->Departure->tm_year + 1900;
				int month = previous == NULL ? FIRST_MONTH : previous->flightData->Departure->tm_mon + 1;
				int day   = previous == NULL ? FIRST_DAY   : previous->flightData->Departure->tm_mday + 1;

				Flight* newFlight = CreateFlight(
					0,
					CreateDate(day, month, year, 0, 0, 0),
					NULL,
					plane->SeatsNumber,
					CloneSeatsFromAirplane(plane),
					plane->ShuttleName
				);

				AddNodeToHashTable(ht, CreateNode(GetYearDayFromFlight(newFlight), newFlight));
				
				return newFlight;

			}
			else {

				// lets check if this flight can hold n passengers

				if (head->flightData->Status == 0) {
					// we need to check if the flight has not taken
					// off yet

					if (CountFreeSeats(head->flightData) >= n) {
						// if true, this flight will do.

						return head->flightData;

					}

				}

			}

			previous = head;

		}

		// very important to advance one column.
		// we could wnd up stuck in the forever loop
		// for a while. (intended pun)
		currentColumnToCheck++;

	}

}

/*
Sets a flight priority
0: not priority
1: priority
*/
void SetFlightPriority(Flight* f, int i) {

	f->IsPriority = i;

}

/*
Sets a seat priority
0: not priority
1: priority
*/
void SetSeatPriority(Seat* s, int i) {

	s->IsPriority = i;

}

/*
Prints flight from a specified date range
*/
void PrintSpecifiedRange(HashTable* flights, struct tm* start, struct tm* finish) {

	if (flights == NULL) {

		return;

	}

	time_t startTime = mktime(start);
	time_t finishTime = mktime(finish);

	for (int i = 0; i < HASH_MAX_SIZE; i++) {

		Node* head = flights->table[i];
		
		while (head != NULL) {
			
			time_t flightTime = mktime(head->flightData->Departure);

			if (flightTime >= startTime && flightTime <= finishTime) {

				PrintFlightPretty(head->flightData);

			}

			head = head->next;
		}
		
	}

}

/*
Updates Status of all flights based on time
*/
void UpdateFlightStatus(HashTable* flights, struct tm* currentdate) {
	
	if (flights == NULL) {

		return;

	}

	time_t current_t = mktime(currentdate);

	for (int i = 0; i < HASH_MAX_SIZE; i++) {

		Node* head = flights->table[i];

		while (head != NULL) {

			time_t flightTime = mktime(head->flightData->Departure);

			if (current_t >= flightTime) {

				struct tm* arrive = CloneStructtm(head->flightData->Departure);
				arrive->tm_hour += 1;
				arrive->tm_min += rand() % 10;

				time_t arrive_t = mktime(arrive);

				head->flightData->Arrive = CloneStructtm(localtime(&arrive_t));
				head->flightData->Status = 1;

				printf("Flight %ld arrived to it's destination! ", head->flightData->ID->value);

				// save to file
				int sucess = SaveFlightToFile(head->flightData);

				if (sucess >= 0) {

					printf("Saved to file.\n", head->flightData->ID->value);

				}

			}

			head = head->next;
		}

	}

}

/*
Save flight to file
*/
int SaveFlightToFile(Flight* flight) {

	if (flight == NULL) {

		return -1;

	}

	char filename[50];

	// using the id instead of the date
	sprintf(filename, "voo%ld.dat", flight->ID->value);

	FILE* f;
	f = fopen(filename, "wb");

	if (f == NULL) {

		return -2;

	}

	// write
	fwrite(flight, sizeof(Flight), 1, f);

	fclose(f);
	f = NULL;
	
	return 0;

}