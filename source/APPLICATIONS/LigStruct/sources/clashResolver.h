// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
#ifndef CLASHRESOLVER_H
#define CLASHRESOLVER_H

#include "base.h"
#include <BALL/KERNEL/bond.h>
#include <BALL/MATHS/angle.h>
#include <BALL/DATATYPE/hashSet.h>

//using namespace OpenBabel;
using namespace BALL;
//using namespace std;

class ConnectionClashResolver
{
public:
//	ClashResolver();
	ConnectionClashResolver( float tolerance = 1.3, int max_rotors = 2);
	~ConnectionClashResolver();
	
	/**
	 * Preconditions: 
	 * 1.) The roots of 'atm1' and 'atm2' are two different BALL::AtomContainers 
	 * 2.) A bond between 'atm1' and 'atm2' exists.
	 * 3.) 'connections' contains all (rotable) fragment-bridging bonds
	 * 3.) The AtomContainer of 'atm1' is the larger one.
	 * 4.) Optimally '_root_large' should not be transformed.
	 * @brief setMolecule, sets the Resolver up to analyze and resolve clashes
	 *        between and within the fragments behind atm1 and atm2.
	 * @param atm1 part of the larger fragment
	 * @param atm2 part of the smaller fragment
	 */
	void setMolecule(Atom& atm1, Atom& atm2, ConnectList& connections, ConnectList *more_rotors=0);
	
	int resolve(bool conserve_large = false);
	
	/**
	 * @brief detect ONLY clashes that occur between the two fragments NOT within
	 * a fragment (we assume that the two should start as clash free). Is a 
	 * wrapper for the call "detectBetweenMolecules( *_large_root, *_small_root)"
	 * 
	 * @return number of clashes found BETWEEN both fragments
	 */
	int detect();
	
	/**
	 * Keep positions of atoms connected to 'atm1' but rotate all atoms that
	 * are connected to 'atm2' around the axis atm1-atm2 about 'angle'
	 * degree/radiant
	 */
	void rotate(Atom& atm1, Atom& atm2, Angle angle);
	
private:
	/// private Methods for detection
	/*
	 * Detect all clashes within 'ac'
	 */
	int detectInMolecule(AtomContainer& ac);
	
	/*
	 * Detect all clashes between 'ac1' and 'ac2' but ignore the clashes occuring
	 * within each fragment.
	 */
	int detectBetweenMolecules(AtomContainer& ac1, AtomContainer& ac2);
	
	/*
	 * check if the two atoms are speparated by at least 3 bonds
	 */
	bool atom3Away(Atom& at1, Atom& at2);
	
	/*
	 * check if actual dist+tolerance is grater than the theoretical vdw-dist
	 */
	bool doClash(Atom& atm1, Atom& atm2);
	
	/// private Methods for resolving
	/*
	 * Try to resolve clashes between large and small fragment by rotating along
	 * the connecting bond.
	 */
	int resolveConnection();
	
	/*
	 * Solve optimally under the constraint of discretized angles. ('steps'
	 * gives the number of angles that are to be tested for each bond)
	 * 
	 * Rotates all bridging bonds in 'small' and 'large'
	 */ 
	int resolveAll( const int& steps );
	
	int resolveAllRecur(const ConnectList::iterator& p, 
											const ConnectList::iterator &p_end, 
											Angle& angle, const int& steps , int &best_cnt);
	/*
	 * Rotate bonds in a fragment 'frag' to remove inter and intra clashes
	 */
	int resolveFragment(AtomContainer& frag , ConnectList &clist);
	
	/*
	 * Helper for 'rotate'
	 */
	void setAtomsToRotate(Atom &start ,Atom &probe, Atom &block, HashSet<Atom *> &result);
	
	Atom* atm_large;
	Atom* atm_small;
	 
	AtomContainer* _large_root;
	AtomContainer* _small_root;

	ConnectList* _small_rotors;
	ConnectList* _large_rotors;
	ConnectList* _all_rotors;
	
	TemplateCoord* _save_large;
	TemplateCoord* _save_small;
	
	const float _tolerance; // tolerance in Anstroem for vdw-dist violation
	int _max_rotations;     // maximum number of bonds to rotate
};


///#TODO: was faster to simply reimplement but here we actually need a class 
/// hierachy
class ClashResolver
{
public:
	ClashResolver( float tolerance = 1.2, int max_rotors = 20);
	~ClashResolver();
	
	/**
	 * @brief setMolecule
	 * @param molecule
	 * @param connections
	 */
	void setMolecule(AtomContainer& molecule, ConnectList& rotors);
	
//	void setMolecule(AtomContainer& molecule);
	
	int resolve();
	
	/**
	 * @brief detect ONLY clashes that occur between the two fragments NOT within
	 * a fragment (we assume that the two should start as clash free). Is a 
	 * wrapper for the call "detectBetweenMolecules( *_large_root, *_small_root)"
	 * 
	 * @return number of clashes found BETWEEN both fragments
	 */
	int detect();
	
	/**
	 * Keep positions of atoms connected to 'atm1' but rotate all atoms that
	 * are connected to 'atm2' around the axis atm1-atm2 about 'angle'
	 * degree/radiant
	 */
	void rotate(Atom& atm1, Atom& atm2, Angle angle);
	
private:
	/// private Methods for detection
	/*
	 * Detect all clashes within 'ac'
	 */
	int detectInMolecule(AtomContainer& ac);
	
	/*
	 * check if the two atoms are speparated by at least 3 bonds
	 */
	bool atom3Away(Atom& at1, Atom& at2);
	
	/*
	 * check if actual dist+tolerance is grater than the theoretical vdw-dist
	 */
	bool doClash(Atom& atm1, Atom& atm2);
	
	/// private Methods for resolving
	/*
	 * Solve optimally under the constraint of discretized angles. ('steps'
	 * gives the number of angles that are to be tested for each bond)
	 * 
	 * Rotates all bridging bonds in 'small' and 'large'
	 */ 
	int resolveAll( const int& steps );
	
	int resolveAllRecur(const ConnectList::iterator& p, 
											const ConnectList::iterator &p_end, 
											Angle& angle, const int& steps );

	
	/*
	 * Helper for 'rotate'
	 */
	void setAtomsToRotate(Atom &start ,Atom &probe, Atom &block, HashSet<Atom *> &result);
	
	AtomContainer* _molecule;

	ConnectList* _rotors;
	
	const float _tolerance;   // tolerance in Anstroem for vdw-dist violation
	int _max_rotations; // maximum number of bonds to rotate
};
#endif // CLASHRESOLVER_H