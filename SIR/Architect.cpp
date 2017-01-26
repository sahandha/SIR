/** 
 * Architect.cpp
 *
 *  Created on: Jan 9, 2015
 *      Author: Sahand
 */


#include "Architect.h"
#include "unistd.h"
#include "Parameters.h"

using namespace std;

Architect::Architect(double t0, double te, double ts, list<Person *> pp, Storage* store):
_Storage(store)
{
	_InitialTime  = t0;
	_EndTime      = te;
	_TimeStep     = ts;
	_CurrentTime  = t0;
	_TimeIndex    = 0;
	_PeoplePtr    = pp;
	PopulationData();
	_generator = new default_random_engine(_RandSeed);
	_introtimeDist = new uniform_int_distribution<int>(introtimeDist1, introtimeDist2);
	for (auto ip = _PeoplePtr.cbegin(); ip != _PeoplePtr.cend();ip++)
    {
		(*ip)->setNeighbors(&_PeoplePtr);
	}
	_SQLBatchSize = 0;
	_SaveIntegerTimes = false;
}



Architect::~Architect() 
{
	delete _generator;
	delete _introtimeDist;
}

// Setters
void Architect::setDomain(Domain *city)
{
	_City = city;
};
void Architect::setPlaces(vector<Place *> places)
{
	_AllPlaces = places;
}
void Architect::setBatchSize(int btchsz)
{
	_SQLBatchSize=btchsz;
};
void Architect::setSaveIntegerTimes(bool siono)
{
	_SaveIntegerTimes = siono;
}

// Getters
double Architect::getCurrentTime()
{
	return _CurrentTime;
}
double Architect::getTimeStep()
{
	return _TimeStep;
}
list<Person*> Architect::getPeople()
{
	return _PeoplePtr;
}
vector<Place*> Architect::getPlaces()
{
	return _AllPlaces;
}
double Architect::getMonthlyTime()
{
	int hour    = floor(_CurrentTime);
	double min  = _CurrentTime - hour;
	
	return ((hour % 30) + min);
}
int Architect::getBatchSize()
{
	return _SQLBatchSize;
}
int Architect::getS()
{
	return _S;
}
int Architect::getI()
{
	return _I;
}
int Architect::getP()
{
	return _P;
}
int Architect::getR()
{
	return _R;
}
int Architect::getD()
{
	return _D;
}
int Architect::getN()
{
	return _N;
}
bool Architect::getSaveIntegerTimes(){
	return _SaveIntegerTimes;
}
Domain* Architect::getDomain()
{
	return _City;
}

// Utilities
void Architect::IncrementTime()
{
	/**
	 * \callergraph
	 */
	_CurrentTime += _TimeStep;
	_TimeIndex++;
}
void Architect::Simulate()
{
	/**
	 * \callergraph
	 *
	 */
	_BirthRate = (_I>0.0)? birthRate1 : birthRate2;
	bool timeIntegerQ = (_SaveIntegerTimes)? (abs(_CurrentTime - round(_CurrentTime)) < _TimeStep/2.):true;
	cout << timeIntegerQ << endl;
		


	int batchctr = 0;
	string statement="";
	int introtime;

	for (double t = 0; t < _EndTime; t += _TimeStep)
    {  //Main Loop

		if (_PeoplePtr.empty())
        {
			cout << "You have run out of people. Enjoy playing god without puppets." << endl;
			return;
		}
		
		int indx  = rand() % (_AllPlaces.size());
		
		introtime = (*_introtimeDist)(*_generator);

		// This block is here to add outside travelers (lightining events).
		if (_CurrentTime != 0 & (fmod(_CurrentTime,introtime)) < 1e-6)
        {
			
			double xmin = (_AllPlaces[indx]->Perimeter)[0][0];
			double xmax = (_AllPlaces[indx]->Perimeter)[0][1];
			double ymin = (_AllPlaces[indx]->Perimeter)[1][0];
			double ymax = (_AllPlaces[indx]->Perimeter)[1][1];
			
			uniform_real_distribution<double> xdist(xmin, xmax);
			uniform_real_distribution<double> ydist(ymin, ymax);
			double x = xdist(*_generator);
			double y = ydist(*_generator);
			AddPerson(x,y);

			x = xdist(*_generator);
			y = ydist(*_generator);
			AddPerson(x,y);
			cout << "==================>>>>>Traveler arrived<<<<<==============" << endl;
		}

		Update(_Storage);

		// Next block adds ppl by the process of birth
		if (_BirthRate >= 1)
		{ // Keep in mind this condition works correctly only for dt=1
			for (int i = 0; i<_BirthRate; i++)
            {
				AddPerson("NewBirth");
				cout << "============>>>>>New birth<<<<============" << endl;
			}
		} else if (_BirthRate>0)
        {
			if (fmod(_CurrentTime/_TimeStep, floor(1/_BirthRate)) == 0 ){
				AddPerson("NewBirth");
				cout << "============>>>>>New birth<<<<============" << endl;
			}
		}
	}

	_Storage->writeSIR();
	cout << "Simulation Complete. Thank you...!" << endl;
}
void Architect::Update(Storage* store)
{
	/**
	 * \callergraph
	 */

	store->saveSIR(_TimeIndex, _CurrentTime, _S, _I, _P, _R, _D);
	//store->startPersonDataSave(_CurrentTime);

	IncrementTime();
	list<Person*> peeps;

	double weight;

	_BirthRate = (_I>0.0)? birthRate1 : birthRate2;



	for (auto pl = _AllPlaces.begin(); pl != _AllPlaces.end(); pl++)
    {
		peeps = *(*pl)->getOccupants();
		for (auto ip = peeps.begin(); ip != peeps.end(); ip++)
        {
			/***
			store ->personDataSave((*ip)->getID(),
														 (*ip)->getTime(),
														 (*ip)->getAge(),
														 (*ip)->getCoordinates(),
														((*ip)->getLocation())->getID(),
														 (*ip)->getState(),
														((*ip)->getInHostDynamics()).getT(),
														((*ip)->getInHostDynamics()).getI(),
														((*ip)->getInHostDynamics()).getV());
			***/
			weight = 1;




			(*ip)->setTime(_CurrentTime);
			
			if ((*ip)->getState()=='D')
            {
				if (_CurrentTime >= (*ip)->getTimeOfDeath()+1)
                {
					Funeral(*ip);
					RemovePerson(*ip);
					delete(*ip);
					if (ip == _PeoplePtr.end()){
						break;
					} 
                    else 
                    {
						ip = ip++;
					}
				}
			}
			else
            {
				(*ip)->setNeighbors(&peeps);
				((*ip)->getInHostDynamics()).setMaxInfLev(0);
			}

			(*ip)->Update();
		}

	}
	//store->endPersonDataSave();
	PopulationData();
}

void Architect::DisplayTime()
{
	int day   = floor(_CurrentTime/24);
	int mhour = floor(_CurrentTime);
	mhour     = mhour % 24;
	int  hour = mhour % 12;
	
	int min   = floor((_CurrentTime - floor(_CurrentTime))*60);
	
	string AmPm = ((mhour < 12)? "AM":"PM");
	cout << "Day " << ((day < 10 )? " ":"") << day <<  ", ";
	cout << ((hour <  10 && hour > 0)? " ":"");
	cout << ((hour ==  0)? 12:hour) << ":";
	cout << ((min  <  10)? "0":"");
	cout << min  << " " << AmPm << endl;
	
}
void Architect::PopulationData()
{
	/**
	 * \callergraph
	 */
	_S = 0;
	_I = 0;
	_P = 0;
	_R = 0;
	_D = 0;
	_N = 0;
	_B = 0;
	
	
	for(auto ip = _PeoplePtr.begin(); ip != _PeoplePtr.end(); ++ip) 
    {
		
		if (((*ip)->getState()) == 'I'){
			_I += 1;
			if ((*ip)->getNewInf()){
				_N += 1;
			}
		}
		else if(((*ip)->getState()) == 'P'){
			_P += 1;
		}
		else if(((*ip)->getState()) == 'S'){
			_S += 1;
		}
		else if(((*ip)->getState()) == 'B'){
			_B += 1;
		}
		else if(((*ip)->getState()) == 'R'){
			_R += 1;
		}
		else if(((*ip)->getState()) == 'D'){
			_D += 1;
		}	
	}
}
void Architect::RemovePerson(Person *p)
{
	/**
	 * \callergraph
	 */
	_PeoplePtr.remove(p);
}
void Architect::PrepDB()
{
	/**
	 * \callergraph
	 */
	// ====================>>>>LocationData<<<========================== //
	// Domain
	

	/***
	_sqlDataPtr->InsertValue("Location",
							"NULL, '1', '" +
							_City->getName() + "', " +
							"'Domain'"     + ", " +
						    to_string((_City->Boundary)[0][0]) + ", " +
							to_string((_City->Boundary)[0][1]) + ", " +
							to_string((_City->Boundary)[1][0]) + ", " +
							to_string((_City->Boundary)[1][1]));
	***/
	for(auto p = _AllPlaces.cbegin(); p != _AllPlaces.cend(); ++p) 
    {
		/*
		_sqlDataPtr->InsertValue("Location",
								"NULL, '"        +
								to_string((*p)->getID()) + "', '" +
								(*p)->getName() + "', '" +
								(*p)->getType() + "', "  +
								to_string(((*p)->Perimeter)[0][0]) + ", " +
								to_string(((*p)->Perimeter)[0][1]) + ", " +
								to_string(((*p)->Perimeter)[1][0]) + ", " +
								to_string(((*p)->Perimeter)[1][1]));
		 */
	}
	// =====================>>>End of LocationData<<<========================= //
	
	// =====================>>>People Data<<<================================= //
	unsigned long ps = _PeoplePtr.size();
	
	cout << "Prepping tables for " << ps << " people. Please wait..." << endl;
	for(auto p = _PeoplePtr.cbegin(); p != _PeoplePtr.cend(); ++p) 
    {

		/*
		_sqlDataPtr->InsertValue("People",
								"NULL, '"        +
								(*p)->getName() + "', " +
								to_string((*p)->getAge()) + ", '"  +
								(*p)->getGender() + "', " +
								to_string(((*p)->getDeafaultLocation())->getID()) + ", " +
								to_string(((*p)->getLocation())->getID()));
		*/
	}
	
	
	// =====================>>>End of People Data<<<========================== //
	
	
}
void Architect::StoreConnections()
{
	map<vector<int>,double>  connections;
	string statement = "";
	for (auto ip = _PeoplePtr.cbegin(); ip != _PeoplePtr.cend(); ip++){
		cout << "Writing Person " << (*ip)->getID() << "'s connections." << endl;
		connections = (*ip)->getConnections();
		if (connections.size() > 0){
			for (auto c = connections.begin(); c != connections.end(); ++c){
				statement = statement + "(" + "NULL, " +
				to_string((c->first)[0]) + ", " +
				to_string((c->first)[1]) + ", " +
				to_string((c->second)) + "),";
			}
			statement.pop_back();
			//_sqlDataPtr -> InsertValue("Connections", statement, true);
		}
		statement = "";
	}
}
Place* Architect::LocFromCoo(double x, double y)
{
	/**
	 * \callergraph
	 */
	for(auto p = _AllPlaces.cbegin(); p != _AllPlaces.cend(); ++p){
		if ((*p)->ContainsQ(x, y)){
			return *p;
		}
	}
	
	cout << "No Homeless people allowed in DiseaseVille!" << endl;
	return _AllPlaces.front();
}
void Architect::AddPerson(Person *p)
{
	/**
	 * \callergraph
	 */
	_PeoplePtr.push_back(p);
	PopulationData();
}
void Architect::AddPerson(double x, double y)
{
	/**
	 * \callergraph
	 */
	unsigned long s = _PeoplePtr.size();
	int id = (int) s + 2;
	Person* p1 = _PeoplePtr.front();
	double dt = (p1->getInHostDynamics()).getdt();
	
	InHostDynamics ihd = InHostDynamics(id,dt, 3, 0, 0.3, 2,44, 40, 100);
	ihd.setBeta(0.1);
	ihd.setDelta(0.1);
	ihd.setP(0.4);
	ihd.setC(0.5);
	ihd.setILRate(0.001);
	
	Place* loc = LocFromCoo(x,y);
	
	vector<Place*> availPlaces = p1->getAvailablePlaces();
	//vector<Place*> availPlaces = _PeoplePtr[randPIdx]->getAvailablePlaces();
	
	Person* p = new Person(id, "Alplego", 40, 'S', ihd, _City, loc, availPlaces);
	
	p->setTravelerQ(false);
	p->setTime(_CurrentTime);
	p->setTimeStep(_TimeStep);
	p->setAgeIncrement(_TimeStep/365);
	p->setNeighbors(&_PeoplePtr);
	
	double coo[2] = {x,y};
	p->setCoordinates(coo);
	p->setHasBeenSick(0);
	p->setMotionStepSize(0.1);
	p->setSociability(1);
	p->setLifeExpectancy(80);

	/*
		_sqlDataPtr->InsertValue("People",
	 "NULL, '" +
	 p->getName() + "', "+
	 to_string(p->getAge()) + ", '" +
	 p->getGender() + "', " +
	 to_string((p->getDeafaultLocation())->getID()) + ", " +
	 to_string((p->getLocation())->getID()));
	 */

}
void Architect::AddPerson(string NewBirth)
{
	/**
	 * \callergraph
	 */
	int indx;
	if ((_AllPlaces.size())>1){
		indx  = rand() % (_AllPlaces.size());
	}
	else{
		indx = 0;
	}

	unsigned long s = _PeoplePtr.size();
	int id = (int) s + 1;
	Person* p1 = _PeoplePtr.front();
	double dt = (p1->getInHostDynamics()).getdt();
	
	InHostDynamics ihd = InHostDynamics(id, dt, 0, 0, 0, 2, 44, 40, 100);
	ihd.setBeta(0.1);
	ihd.setDelta(0.1);
	ihd.setP(0.4);
	ihd.setC(1.0);
	ihd.setILRate(0.001);
	

	vector<Place*> availPlaces = p1->getAvailablePlaces();

	
	Person* p = new Person(id, "Alplego", 0, 'B', ihd, _City, _AllPlaces[indx], availPlaces);

	p->setTime(_CurrentTime);
	p->setTimeStep(_TimeStep);
	p->setAgeIncrement(_TimeStep/365);
	p->setNeighbors(&_PeoplePtr);
	p->setMotionStepSize(0.01);
	p->setSociability(0.5);
	p->setLifeExpectancy(80);
	//p->setTravelerQ(true);
	p->setHasBeenSick(0);
	/*
		_sqlDataPtr->InsertValue("People",
	 "NULL, '" +
	 p->getName() + "', "+
	 to_string(p->getAge()) + ", '" +
	 p->getGender() + "', " +
	 to_string((p->getDeafaultLocation())->getID()) + ", " +
	 to_string((p->getLocation())->getID()));
	 */
	AddPerson(p);

}
void Architect::Funeral(Person* p)
{
	/**
	 * \callergraph
	 */
	(p->getLocation())->removePerson(p);
}
bool Architect::memberQ(vector<int>* v, int value)
{
	return (find(v->begin(), v->end(),value)!=v->end());
}
