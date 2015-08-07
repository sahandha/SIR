/*
 * Architect.cpp
 *
 *  Created on: Jan 9, 2015
 *      Author: Sahand
 */

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include "Architect.h"
#include "Person.h"
#include "Storage.h"

using namespace std;

Architect::Architect(double t0, double te, double ts,
		vector<Person *> pp, string store, Storage* d):
	dataPtr(d)
{

	InitialTime  = t0;
	EndTime      = te;
	TimeStep     = ts;
	CurrentTime  = t0;
	TimeIndex    = 0;
	PeoplePtr    = pp;
	Store        = store;
    PopulationData();
}

Architect::Architect(double t0, double te, double ts,
                     vector<Person *> pp, string store, SQLStorage* d):
sqlDataPtr(d)
{
    
    InitialTime  = t0;
    EndTime      = te;
    TimeStep     = ts;
    CurrentTime  = t0;
    TimeIndex    = 0;
    PeoplePtr    = pp;
    Store        = store;
    PopulationData();
}


Architect::Architect(double t0, double te, double ts, vector<Person *> pp, string store)
{
    
    InitialTime  = t0;
    EndTime      = te;
    TimeStep     = ts;
    CurrentTime  = t0;
    TimeIndex    = 0;
    PeoplePtr    = pp;
    Store        = store;
    PopulationData();
}



Architect::~Architect() {

}
// Setters

void Architect::setDomain(Domain city){
    City = city;
};
void Architect::setHomes(vector<Place*> homes){
    Homes = homes;
};
void Architect::setSchools(vector<Place*> schools){
    Schools = schools;
};
void Architect::setWorks(vector<Place*> works){
    Works = works;
};
void Architect::setCemetaries(vector<Place*> cemeteries){
    Cemeteries = cemeteries;
};

// Getters
double Architect::getCurrentTime(){
	return CurrentTime;
}
double Architect::getTimeStep(){
	return TimeStep;
}
vector<Person*> Architect::getPeople(){
	return PeoplePtr;
}
double Architect::getDailyTime(){
	int hour    = floor(CurrentTime);
	double min  = CurrentTime - hour;

	return ((hour % 24) + min);
}
int Architect::getS(){
    return S;
}
int Architect::getI(){
    return I;
}
int Architect::getP(){
    return P;
}
int Architect::getR(){
    return R;
}

// Utilities
void Architect::IncrementTime(){
	CurrentTime += TimeStep;
	TimeIndex++;
}
void Architect::Simulate(){
	if (Store == "FileSystem"){
		dataPtr->citySave();
		dataPtr->homeSave();
		dataPtr->workSave();
		dataPtr->schoolSave();
        dataPtr->cemeterySave();
		for (double t = 0; t < EndTime; t += TimeStep){
			Update(t, dataPtr);
		}
        dataPtr->writeSIR();
	}
    else if (Store == "MYSQL"){
        PrepDB();
        for (double t = 0; t < EndTime; t += TimeStep){
            Update(t, sqlDataPtr);
            cout << "=========>>>>> Time " << t << " of " << EndTime << " <<<<<=================="<< endl;
            sqlDataPtr-> InsertValue("HistoryData",
                                  "NULL, " +
                                  to_string(t) + ", " +
                                  to_string(S) + ", " +
                                  to_string(I) + ", " +
                                  to_string(P) + ", " +
                                  to_string(R) + ", " +
                                  to_string(D)
                                  );
            
        }
        
    }
	else{
		for (double t = 0; t < EndTime; t += TimeStep){
			Update(t);
		}
	}
    cout << "Stick a fork in me sucker!" << endl;
}
void Architect::Update(double t, Storage* data){
    
	data->saveSIR(TimeIndex, CurrentTime, S, I, P, R, D);
	data->startMovieSave(CurrentTime);
	IncrementTime();
	for (auto ip = PeoplePtr.cbegin(); ip != PeoplePtr.cend(); ++ip){
        ((*ip)->getInHostDynamics()).setMaxInfLev(0);
		data->movieSave((*ip)->getID(),
                        (*ip)->getName(),
                        (*ip)->getTime(),
                        (*ip)->getCoordinates(),
                       ((*ip)->getLocation())->getName(),
                        (*ip)->getState(),
                        (*ip)->getHastBeenSick(),
                       ((*ip)->getInHostDynamics()).getT(),
                       ((*ip)->getInHostDynamics()).getI(),
                       ((*ip)->getInHostDynamics()).getV(),
                       ((*ip)->getInHostDynamics()).getMaxInfLev(),
                        ((*ip)->getSIConnections()),
                        ((*ip)->getSIConnectionsHist()),
                        ((*ip)->getAllConnections()),
                        ((*ip)->getAllConnectionsHist()));
        
		(*ip)->setTime(CurrentTime);
		if ((*ip)->IsSingleLocation) {
			(*ip)->Move2((rand() % 360),5);
		}else{
			(*ip)->Move((rand() % 360),2, "Travel");
		}
		(*ip)->UpdateDiseaseWithInHost();
	}
	data->endMovieSave();
    PopulationData();
}
void Architect::Update(double t, SQLStorage* data){
    
    IncrementTime();
    for (auto ip = PeoplePtr.cbegin(); ip != PeoplePtr.cend(); ++ip){
        ((*ip)->getInHostDynamics()).setMaxInfLev(0);
        data-> InsertValue("PersonValues",
                           "NULL, " +
                           to_string((*ip)->getID()) + ", " +
                           to_string((*ip)->getTime()) + ", " +
                           to_string((*ip)->getCoordinates()[0]) + ", " +
                           to_string((*ip)->getCoordinates()[1]) + ", " +
                           to_string(((*ip)->getLocation())->getID()) + ", '" +
                           (*ip)->getState() + "', " +
                           to_string((*ip)->getHastBeenSick()) + ", " +
                           to_string(((*ip)->getInHostDynamics()).getT()) + ", " +
                           to_string(((*ip)->getInHostDynamics()).getI()) + ", " +
                           to_string(((*ip)->getInHostDynamics()).getV()) + ", " +
                           to_string(((*ip)->getInHostDynamics()).getMaxInfLev())
                           );
        
        (*ip)->setTime(CurrentTime);
        if ((*ip)->IsSingleLocation) {
            (*ip)->Move2((rand() % 360),5);
        }else{
            (*ip)->Move((rand() % 360),2, "Travel");
        }
        (*ip)->UpdateDiseaseWithInHost();

    }
    PopulationData();
}
void Architect::Update(double t){
	
    IncrementTime();
	for (auto ip = PeoplePtr.cbegin(); ip != PeoplePtr.cend(); ++ip){
		(*ip)->setTime(CurrentTime);
		if ((*ip)->IsSingleLocation) {
			(*ip)->Move2((rand() % 360),200);
		}else{
			(*ip)->Move((rand() % 360),5);
		}
		(*ip)->UpdateDisease();
	}
    PopulationData();
}
void Architect::DisplayTime(){
	int day   = floor(CurrentTime/24);
	int mhour = floor(CurrentTime);
	mhour     = mhour % 24;
	int  hour = mhour % 12;

	int min   = floor((CurrentTime - floor(CurrentTime))*60);

	string AmPm = ((mhour < 12)? "AM":"PM");
	cout << "Day " << ((day < 10 )? " ":"") << day <<  " ";
	cout << ((hour <  10 && hour > 0)? " ":"");
	cout << ((hour ==  0)? 12:hour) << ":";
	cout << ((min  <  10)? "0":"");
	cout << min  << " " << AmPm << endl;

}
void Architect::PopulationData(){
    S = 0;
    I = 0;
    P = 0;
    R = 0;
    D = 0;
    for(auto ip = PeoplePtr.cbegin(); ip != PeoplePtr.cend(); ++ip) {
        if (((*ip)->getState()) == 'I'){
            I += 1;
        }
        else if(((*ip)->getState()) == 'P'){
            P += 1;
        }
        else if(((*ip)->getState()) == 'S'){
            S += 1;
        }
        else if(((*ip)->getState()) == 'R'){
            R += 1;
        }
        else{
            D += 1;
        }
    }
}
void Architect::AddPerson(Person *p){
    PeoplePtr.push_back(p);
    PopulationData();
}
void Architect::PrepDB(){

    
    // ====================>>>>LocationData<<<========================== //
    // Domain
    sqlDataPtr->InsertValue("Location",
                   "NULL, '" +
                   City.getName() + "', " +
                   "'Domain'"     + ", " +
                   to_string((City.Boundary)[0][0]) + ", " +
                   to_string((City.Boundary)[0][1]) + ", " +
                   to_string((City.Boundary)[1][0]) + ", " +
                   to_string((City.Boundary)[1][1]));
    
    // Homes
    for(auto h = Homes.cbegin(); h != Homes.cend(); ++h) {
        sqlDataPtr->InsertValue("Location",
                       "NULL, '"        +
                       (*h)->getName() + "', '" +
                       (*h)->getType() + "', "  +
                       to_string(((*h)->Perimeter)[0][0]) + ", " +
                       to_string(((*h)->Perimeter)[0][1]) + ", " +
                       to_string(((*h)->Perimeter)[1][0]) + ", " +
                       to_string(((*h)->Perimeter)[1][1]));
    }
    // Works
    for(auto h = Works.cbegin(); h != Works.cend(); ++h) {
        sqlDataPtr->InsertValue("Location",
                       "NULL, '"        +
                       (*h)->getName() + "', '" +
                       (*h)->getType() + "', "  +
                       to_string(((*h)->Perimeter)[0][0]) + ", " +
                       to_string(((*h)->Perimeter)[0][1]) + ", " +
                       to_string(((*h)->Perimeter)[1][0]) + ", " +
                       to_string(((*h)->Perimeter)[1][1]));
    }
    // Schools
    for(auto h = Schools.cbegin(); h != Schools.cend(); ++h) {
        sqlDataPtr->InsertValue("Location",
                       "NULL, '"        +
                       (*h)->getName() + "', '" +
                       (*h)->getType() + "', "  +
                       to_string(((*h)->Perimeter)[0][0]) + ", " +
                       to_string(((*h)->Perimeter)[0][1]) + ", " +
                       to_string(((*h)->Perimeter)[1][0]) + ", " +
                       to_string(((*h)->Perimeter)[1][1]));
    }
    // Cemeteries
    for(auto h = Cemeteries.cbegin(); h != Cemeteries.cend(); ++h) {
        sqlDataPtr->InsertValue("Location",
                       "NULL, '"       +
                       (*h)->getName() + "', '" +
                       (*h)->getType() + "', "  +
                       to_string(((*h)->Perimeter)[0][0]) + ", " +
                       to_string(((*h)->Perimeter)[0][1]) + ", " +
                       to_string(((*h)->Perimeter)[1][0]) + ", " +
                       to_string(((*h)->Perimeter)[1][1]));
    }
    
    // =====================>>>End of LocationData<<<========================= //
    
    // =====================>>>People Data<<<================================= //
    unsigned long ps = PeoplePtr.size();
    
    cout << "Prepping tables for " << ps << " people. Please wait..." << endl;
    for(auto p = PeoplePtr.cbegin(); p != PeoplePtr.cend(); ++p) {

        sqlDataPtr->InsertValue("People",
                                "NULL, '"        +
                                (*p)->getName() + "', " +
                                to_string((*p)->getAge()) + ", '"  +
                                (*p)->getGender() + "', " +
                                to_string(((*p)->getHome())->getID()) + ", " +
                                to_string(((*p)->getLocation())->getID()));
    }
    
    
    // =====================>>>End of People Data<<<========================== //
    
    
}