#include "Person.h"
#include "parameters.h"

Person::Person(int id, string name, double age, char state, InHostDynamics ihd, Domain* city, Place* location, vector<Place*> availplaces):
_ihdynamics(ihd),
_City(city),
_Location(location),
_AvailablePlaces(availplaces)
{
/**
 * \callergraph
 * ### Things that are set using member initialization:
 * + _ihdynamics
 * + _City
 * + _Location
 * + _AvailablePlaces
 *
 * ### Things that are set whithin the constructor:
 * + _generator
 * + _ID
 * + _Age
 * + _Name
 * + _State
 */

	_generator = new default_random_engine(_RandSeed);
	setID(id);
	setAge(age);
	setName(name);
	setState(state);
	_Time = 0;
	_TimeInfected = 0;
	setGender('M');
	setLocation(location);
	setDefaultLocation(location);
	setAgeInteraction();
	setAgeGroupRules();
	setAgeGroup();

}// end constructor

// Setters
void Person::setID(int id){
	_ID = id;
}
void Person::setX(double x){
	_X = x;
}
void Person::setY(double y){
	_Y = y;
}
void Person::setTimeStep(double ts){
	_TimeStep = ts;
}
void Person::setAge(double age){
	_Age = age;
}
void Person::setAgeIncrement(double ai){
	_AgeIncrement = ai;
}
void Person::setHasBeenSick(int hbs){
	_HasBeenSick = hbs;
}
void Person::setGender(char g){
	_Gender = g;
}
void Person::setName(string name){
	_Name = name;
}
void Person::setCoordinates(double coordinates[2]){
	double x = coordinates[0];
	double y = coordinates[1];
	_Coordinates[0] = x;
	_Coordinates[1] = y;
	setX(x);
	setY(y);
//	if (!_Location->ContainsQ(x, y)){
//		setLocation(_Location);
//	}
//	else{
//		_Coordinates[0] = x;
//		_Coordinates[1] = y;
//		setX(x);
//		setY(y);
//	}

}
void Person::setState(char state){
	_State = state;
}
void Person::setLocation(Place* location){

	double xmin = location->Perimeter[0][0];
	double xmax = location->Perimeter[0][1];
	double ymin = location->Perimeter[1][0];
	double ymax = location->Perimeter[1][1];

	uniform_real_distribution<double> xdist(xmin, xmax);
	uniform_real_distribution<double> ydist(ymin, ymax);

	double x = xdist(*_generator);
	double y = ydist(*_generator);

	_Location = location;
	_Location->addPerson(this);

	double Co[2] = {x,y};

	while(!_Location->ContainsQ(x, y)){
		x = xdist(*_generator);
		y = ydist(*_generator);
		Co[0] = x; Co[1] = y;
	};

	setCoordinates(Co);
}
void Person::setDefaultLocation(Place* location){
	_DefaultLocation = location;
}
void Person::setTime(double t){
	_Time = t;
}
void Person::setMotionStepSize(double ss){
	_MotionStepSize = ss;
}
void Person::setInfectionPeriod(){
	normal_distribution<double> distribution(_ihdynamics.getAverageInfectionPeriod(),_InfectionVar);
	
	double randnum  = distribution(*_generator);
	randnum = (randnum < 0)? 0:randnum;
	_InfectionPeriod = floor(randnum);
}
void Person::setIncubationPeriod(){
	normal_distribution<double> distribution(_ihdynamics.getAverageIncubationPeriod(),_IncubationVar);
	double randnum  = distribution(*_generator);
	randnum = (randnum < 0)? 0:randnum;
	_IncubationPeriod = floor(randnum);
}
void Person::setRecoveryPeriod(){
	normal_distribution<double> distribution(_ihdynamics.getAverageRecoveryPeriod(),_RecoveryVar);
	double randnum  = distribution(*_generator);
	randnum = (randnum < 0)? 0:randnum;
	_RecoveryPeriod = floor(randnum);
}
void Person::setLifeExpectancy(double le){
	_LifeExpectancy = le;
}
void Person::setInHostDynamics(InHostDynamics ihd){
	_ihdynamics = ihd;
}
void Person::setInfVar(int var){
	_InfectionVar = var;
};
void Person::setIncVar(int var){
	_IncubationVar = var;
};
void Person::setRecVar(int var){
	_RecoveryVar = var;
};
void Person::setTravelerQ(bool tq){
	_TravelerQ = tq;
};
void Person::setNeighbors(list<Person *> *n){
	_Neighbors = n;
}
void Person::setAgeInteraction(){
	_AgeInteraction["CC"]=0.1;
	_AgeInteraction["TT"]=0.1;
	_AgeInteraction["CT"]=0;
	_AgeInteraction["YY"]=0.1;
	_AgeInteraction["CY"]=0.1;
	_AgeInteraction["AT"]=0.1;
	_AgeInteraction["AA"]=0.1;
	_AgeInteraction["AC"]=0;
	_AgeInteraction["TY"]=0;
	_AgeInteraction["SS"]=0.1;
	_AgeInteraction["CS"]=0;
	_AgeInteraction["ST"]=0;
	_AgeInteraction["SY"]=0.1;
}
void Person::setAgeGroup(){
	_AgeGroup = _AgeGroupRules[floor(_Age)];
}
void Person::setSociability(double s){
	_Sociability = s;
}
void Person::setAgeGroupRules(){
	for (int i = 0; i <= 100; i++){
		if (i<10){
			_AgeGroupRules[i] = 'C';
		}
		else if (i<20){
			_AgeGroupRules[i] = 'T';
		}
		else if (i<30){
			_AgeGroupRules[i] = 'Y';
		}
		else if(i<60){
			_AgeGroupRules[i] = 'A';
		}
		else{
			_AgeGroupRules[i] = 'S';
		}
	}
}

// Getters
int Person::getID(){
	return _ID;
}
double Person::getX(){
	return _X;
}
double Person::getY(){
	return _Y;
}
double Person::getTimeStep(){
	return _TimeStep;
}
double Person::getAge(){
	return _Age;
}
double Person::getMotionStepSize(){
	return _MotionStepSize;
}
int Person::getHasBeenSick(){
	return _HasBeenSick;
}
char Person::getGender(){
	return _Gender;
}
int Person::getInfectionPeriod(){
	return _InfectionPeriod;
}
int Person::getIncubationPeriod(){
	return _IncubationPeriod;
}
int Person::getRecoveryPeriod(){
	return _RecoveryPeriod;
}
int Person::getInfVar(){
	return _InfectionVar;
}
int Person::getIncVar(){
	return _IncubationVar;
}
int Person::getRecVar(){
	return _RecoveryVar;
}
double Person::getLifeExpectancy(){
	return _LifeExpectancy;
}
Domain* Person::getDomain(){
	return _City;
}
double* Person::getCoordinates(){
	return _Coordinates;
}
double Person::getTime(){
	return _Time;
}
double Person::getTimeOfDeath(){
	return _TimeOfDeath;
}
char Person::getState() const{
	return _State;
}
bool Person::getNewInf(){
	return _NewInf;
}
string Person::getName() const{
	return _Name;
}
Place* Person::getLocation(){
	return _Location;
}
Place* Person::getDeafaultLocation(){
	return _DefaultLocation;
}
vector<Place*> Person::getAvailablePlaces(){
	return _AvailablePlaces;
}
list<Person*>* Person::getNeighbors(){
	return _Neighbors;
}
InHostDynamics Person::getInHostDynamics() const{
	return _ihdynamics;
}
bool Person::getTraverlerQ(){
	return _TravelerQ;
}
char Person::getAgeGroup(){
	return _AgeGroup;
}
double Person::getSociability(){
	return _Sociability;
}
map<vector<int>,double> Person::getConnections(){
	return _Connections;
}
vector<int> Person::getConnectionsi(){
	return _Connectionsi;
}

vector<vector<int>> Person::getWeightedConnections(){
	return _WeightedConnections;
}

void Person::clearConnections(){
	_Connectionsi.clear();
	_WeightedConnections.clear();
}

void Person::Update(){
	/**
	 * \callergraph
	 * \todo
	 */
	//_Connections = "";
	//_Connectionsi.clear();
	_Age += _AgeIncrement;

	if (_State != 'D'){
		InteractWithOthers();
		Move();
		UpdateDiseaseWithInHost();

		if (_Age < 0.1){
			setState('B');
			_ihdynamics.setT(0);
		}
		else if ((_Age >= 0.1) & (_Age - 0.1 < 15)){
			if (getState() == 'B'){
				setState('S');
				_ihdynamics.setT(3);
			}
		}
		else if (_Age >= _LifeExpectancy){
			Die();
		}
	}
}

// Utilities
void Person::InteractWithOthers(){
	/**
	 * \callergraph
	 * 
	 * This function is responsible for two things:
	 *	1. Compute the effect of other people in the motion. 
	 *	2. Compute the effect of others in how much virions the person collects.
	 *
	 */
	
	double motionBias[2];
	double r[4];
	double dist;
	//double theta;
	vector<int> con (2);
	con[0]=_Age;
	vector<int> connections = getConnectionsi();
	vector<int> weightedConnection;
	double weight;

	clearConnections();

	_TotalVirion = 0;
	_MotionBiasX = 0;
	_MotionBiasY = 0;
	weight = 10;
	for(auto ip = _Neighbors->cbegin(); ip != _Neighbors->cend(); ++ip){
		CartesianDistance(*ip,r);
		dist  = sqrt(r[0]*r[0]+r[1]*r[1]);//r[0];
																			//theta = atan2(r[1],r[0]);//r[1];

		if (dist != 0 & dist < criticalDistance){
			weightedConnection.clear();
			computeMotionEffect(r,(*ip)->getAgeGroup(),motionBias);
			//computeMotionEffect(r,(int) (*ip)->getAge(),motionBias);
			_MotionBiasX += motionBias[0];
			_MotionBiasY += motionBias[1];
		}
		if (dist != 0 & dist < criticalDistanceD){
			_TotalVirion += 10*((*ip)->_ihdynamics.getV())/(dist*dist);
			_Connectionsi.push_back((*ip)->getID());
			con[1] = (*ip)->getAge();
			sort(con.begin(), con.end());
			_Connections[con] = _Connections[con]+_TimeStep;
		}
	}
}
void Person::computeMotionEffect(double* distVector, int ag, double * r){
	/**
	 * \callergraph
	 * \todo
	 */

	double G = (abs(_Age-ag)<1)? 0.3:-0.1;
	double critDist = 0.1; //This is so that the force doesn't grow very large

	r[0] = (abs(distVector[0])<critDist)? 0:distVector[2]*pow(_TimeStep,2)*(_Sociability*G/pow(abs(distVector[0]),2));//*cos(distVector[1]);
	r[1] = (abs(distVector[1])<critDist)? 0:distVector[3]*pow(_TimeStep,2)*(_Sociability*G/pow(abs(distVector[1]),2));//*sin(distVector[1]);
}
void Person::computeMotionEffect(double* distVector, char ag, double * r){
	/**
	 * \callergraph
	 * \todo
	 */
	string interType = string()+_AgeGroup+ag;
	sort(interType.begin(),interType.end());
	double G = _AgeInteraction[interType];
	double critDist = 0.1; //This is so that the force doesn't grow very large
	r[0] = (abs(distVector[0])<critDist)? 0:distVector[2]*pow(_TimeStep,2)*(_Sociability*G/pow(abs(distVector[0]),2));//*cos(distVector[1]);
	r[1] = (abs(distVector[1])<critDist)? 0:distVector[3]*pow(_TimeStep,2)*(_Sociability*G/pow(abs(distVector[1]),2));//*sin(distVector[1]);
}
void Person::Move(){
	/**
	 * \callergraph
	 * \todo
	 */

	normal_distribution<double> xdist(_MotionBiasX,_MotionStepSize);
	normal_distribution<double> ydist(_MotionBiasY,_MotionStepSize);
	double rx = xdist(*_generator);
	double ry = ydist(*_generator);

	double x,y;

	if (motionBiasQ){
		x = rx + _MotionBiasX;
		y = ry + _MotionBiasY;
	}
	else {
		x = rx;
		y = ry;
	}

	if ((_State == 'P' | _State == 'I') & varyStepSize){
		Move(x/3,y/3, "IslandHopper");
	}else{
		Move(x,y, "IslandHopper");
	}
}
void Person::Move(double xr, double yr, string motionType){
	/**
	 * \callgraph
	 */

	/*
	if (_State == 'D') {
		if (_Location->getType()=="Cemetery"){
			return;
		}
		for (auto L = _AvailablePlaces.cbegin(); L != _AvailablePlaces.cend(); L++){
			if ((*L)->getType()=="Cemetery") {
				setLocation(*L);
				return;
			}
		}
	}*/

	double x = _X + xr; // r*cos(theta);
	double y = _Y + yr; //*sin(theta);
	double co[2] = {x,y};

/*
	if (_Time != 0 & (fmod(_Time,50)) < 1e-3){
		if (_TravelerQ & (_State != 'D')){
			cout << "============= We have a Traverler ======================" << endl;
			int lid = rand() % (_AvailablePlaces.size()-2) + 1;
			for (auto L = _AvailablePlaces.cbegin(); L != _AvailablePlaces.cend()-1; L++){
				if (((*L)->getID()==lid)){
					setLocation(*L);
				}
			}

		}
	}
*/


	if(_Location->ContainsQ(x, y)){
		setCoordinates(co);
	}
}
void Person::UpdateDiseaseWithInHost(){
	/**
	 * \callergraph
	 * \todo
	 */
	_ihdynamics.setT0(_Time);
	_ihdynamics.setNE(_TotalVirion);
	_ihdynamics.Simulate();
	
	if ((getState()=='R') & !(getHasBeenSick())) {
		if (_ihdynamics.getT() > 1.8){
			setState('S');
		}
	}
	else if (getState() == 'S'){
		if (_ihdynamics.getI() > 0.2){
			setState('I');
			setMotionStepSize(_MotionStepSize/2);
			_NewInf = true;
		}
	}
	else if (getState() == 'I'){
		_NewInf = false;
		if (_ihdynamics.getI() > 0.3 & _ihdynamics.getI() < 2.9 & _HasBeenSick==0){
			setState('P');
			setHasBeenSick(1);
			_ihdynamics.HasBeenSick = 1;
		}else if (_ihdynamics.getI() > 2.9){
			Die();
		}
		else if (_ihdynamics.getI() < 0.1 & _HasBeenSick == 1){
			setState('R');
			setMotionStepSize(_MotionStepSize*2);
			//setHasBeenSick(0);
			//_ihdynamics.HasBeenSick = 1;
		}
	}
	else if (getState() == 'P'){
		_NewInf = false;
		if (_ihdynamics.getI() + 0.01 < _ihdynamics.getMaxInfLev()){
			setHasBeenSick(1);
			_ihdynamics.HasBeenSick = 1;
		}
		if (_ihdynamics.getI() > 2.9){
			Die();
		}
		else if (_ihdynamics.getI() < 0.2){
			setState('I');
		}
	}
	else if (getState() == 'R'){
		_NewInf = false;
		if (_ihdynamics.getI() < 0.1){
			//setHasBeenSick(0);
			//_ihdynamics.HasBeenSick = 0;
		}
	}
	
	//_neigbors.clear();
}
void Person::CartesianDistance(Person* p, double *r){
	/**
	 * \callergraph
	 * 
	 * + This function returns an array where the first two are the
	 * x and y distances of this person from the other. 
	 * + The third and fourth positions are the sign of the difference in x,y.
	 * + If _this_ person is to the right of the _other_, the sign in x is +.
	 * + If _this_ person is above the _other_, then sign for y is +.
	 *
	 */
	// order matters here.
	double xdiff = p->getX() - _X;
	double ydiff = p->getY() - _Y;

	r[0] = xdiff; //sqrt(pow(xdiff,2) + pow(ydiff,2));
	r[1] = ydiff; //atan2(ydiff, xdiff);
	r[2] = (xdiff > 0)? 1:-1;
	r[3] = (ydiff > 0)? 1:-1;
};
double Person::Distance(Person* p){
	/**
	 * \callergraph
	 * This is begin deprecated.
	 * \todo Go through all functions and make sure there is no longer a need for this.
	 */
	double p1x = _Coordinates[0];
	double p1y = _Coordinates[1];
	
	double p2x = (p->getCoordinates())[0];
	double p2y = (p->getCoordinates())[1];
	
	return (pow((p2x-p1x),2) + pow((p2y - p1y),2));
}
void Person::Die(){
	/**
	 * \callergraph
	 */
	setState('D');
	_TimeOfDeath = _Time;
	Move(0, 0, "IslandHopper");
}

bool Person::memberQ(vector<int>* v, int value){
	return (find(v->begin(), v->end(),value)!=v->end());
}

bool Person::operator=(Person &A) const{
	return (this->getState() == A.getState());
}
bool Person::operator == (const Person& p) const {
	return (p._ID == this->_ID);
}
Person::~Person(){
	delete _generator;
	//delete this;
}
