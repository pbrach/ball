#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "basic.h"

using namespace OpenBabel;
using namespace BALL;
using namespace std;

///####################### 3 D    A S S E M B L Y ##############################
	

void buildLinker(vector< Fragment* >& linker_lst)
{
	
}

/// ------ check if for every atom in list1 a matching atom in list2 
/// ------ can be found
bool allMatch(Molecule* li1, Molecule* li2)
{
	AtomIterator at1 = li1->beginAtom();
	for (; +at1 ; at1++)
	{
		bool b = false;
		
		AtomIterator at2 = li2->beginAtom();

		for (; +at2; at2++)
		{
			if (at1->getDistance(*at2) < 0.8){ // epsilon set to 0.8 A
				b = true;
				
				break;
			}
		}
		if (!b)
			return false;
	}
	return true;
}


// compare element+bondorder annotations:
bool compare(pair<String,Atom*>& a, pair<String,Atom*>& b)
{
	return a.first < b.first;
}


/// ------- get connection site
/// 'atm' the atom spanning the site and 'partner' the atom
/// on the other side of the bond that is to be formed.
/// 
int getSite(Atom* atm, vector< Atom* >& site, Atom* partner, String& key)
{
	key = atm->getElement().getSymbol();
	
	// insert central atom as first:
	site.push_back(atm);
	
	// determine the key and the order for atoms
	Atom::BondIterator b_it = atm->beginBond();
	vector< pair<String,Atom*> > elements;
	
	for(; b_it != atm->endBond(); b_it++)
	{
		Atom* tmp_atm = b_it->getBoundAtom(*atm); // get neighbors
		
		String elem = tmp_atm->getElement().getSymbol();
		elem += String(b_it->getOrder());
		
		elements.push_back( make_pair( elem, tmp_atm) );
	}
	
	// also add the partner atom for the new bond:
	elements.push_back( make_pair( String(partner->getElement().getSymbol())+ String(1), partner) );
	
	// sort identifers, but keep central atom as first:
	sort( elements.begin(), elements.end(), compare);
	
	// set the next two atoms that are NOT the partner
	// as additional points for a 3 point match site:
	int pos = -1;
	for(int i = 0; i < elements.size(); i++)
	{
		Atom* tmp = elements[i].second;

		if(tmp != partner)
			site.push_back(tmp);
		
		if(tmp == partner)
		{
			pos = i+1;
		}
	}
	
	// create the key:
	vector< pair<String, Atom*> >::iterator el_it = elements.begin();
	for(; el_it !=elements.end(); el_it++)
	{
		key += ((*el_it).first);
	}
	return pos;
}


/// get transformation vector to move atom 2 so that it has the correct distance
/// to atom 1
Vector3 getDiffVec(Atom* atm1, Atom* atm2, boost::unordered_map <String, float > std_bonds)
{
	String key = atm1->getElement().getSymbol();
	key += atm2->getElement().getSymbol();
	float bond_len = std_bonds[key];
	
	Vector3 diff_vec = atm1->getPosition() - atm2->getPosition();
	float diff_len = diff_vec.getLength() - bond_len;
	
	diff_vec.normalize();
	return (diff_vec * diff_len);
}


/// ------- merge two connection templates to a final template
/// the final template will only contain 6 atoms, 3 for each end
/// starting at position 0 and 3 with the molecules that are to be
/// connected, and then the ordered next two neighbors
void mergeTemplates(Molecule* mol1, int pos1, Molecule* mol2, int pos2, boost::unordered_map<String, float> std_bonds)
{
	Atom* aTarget = mol1->getAtom(pos1);
	Atom* bTarget = mol2->getAtom(pos2);
	Atom* atm1 = mol1->getAtom(0);
	Atom* atm2 = mol2->getAtom(0);

	// got no time to take care of the possible sign errors thus simply use 3-point-matching:
	Vector3& ptA1 = atm1->getPosition();
	Vector3& ptA2 = aTarget->getPosition();
	Vector3& ptB1 = bTarget->getPosition();
	Vector3& ptB2 = atm2->getPosition();

	Matrix4x4 rot_matrix = StructureMapper::matchPoints(ptB1, ptB2, Vector3(), ptA1, ptA2, Vector3());
	
	// transfrom the 2nd template
	TransformationProcessor transformer(rot_matrix);
	mol2->apply(transformer);
	
	// fix bond length
	Vector3 bond_fix = getDiffVec(atm1, atm2, std_bonds);
	TranslationProcessor t_later(bond_fix);
	mol2->apply(t_later);

	// remove the two connection partner atoms, they are redundant now:
	mol1->remove(*aTarget);
	mol2->remove(*bTarget);
}

/// Helper to get a single key component
String getBondName(Atom* atm, Atom* partner)
{
	String name = atm->getElement().getSymbol();
	name += atm->getBond(*partner)->getOrder();
	return name;
}

/// TODO: improve! use the already better version of 'align' from connectionRMSDFilter
/// (Structurally) align a connection site to a template and return the 
/// transformation matrix
/// 
/// What it does: finds a transformation from 'site' to 'templ', that 
/// fits all atoms in both sets. For that a 3 point match searched that fulfills
/// the condition
/// 
/// NOTE: 'all atoms fit' currently means only that positions match, order 
///       and element are neglected for now, but this seems to be still a good
///       approximation.
Matrix4x4 align(vector< Atom* >& site, Molecule* templ)
{
	Matrix4x4 result;
	Vector3 frag1, frag2, frag3, tem1, tem2, tem3;
	
	// set the two center atoms:
	frag1 = site[0]->getPosition();
	tem1  = templ->getAtom(0)->getPosition();
	
	/// simple solution for only one neighbor:
	if(site.size() == 2)
	{
		// got no time to take care of the possible sign errors thus simply use 3-point-matching:
		frag2 = site[1]->getPosition();
		tem2  = templ->getAtom(1)->getPosition();
		return StructureMapper::matchPoints(frag1, frag2, Vector3(), tem1, tem2, Vector3());
	}
	
	/// see if some atoms differ in element and order:
	// find the unique elements, via their 'bondName'
	boost::unordered_map<String, int> el_map;
	for (int i = 1; i < site.size(); i++)
	{
		el_map[ getBondName(site[i], site[0]) ] += 1;
	}
	
	/// try to assign the unique atom if one exists:
	if (el_map.size() != 1)
	{
		vector< Atom* > unique_atm;
		vector< int > unique_pos;
		vector< String > unique_names;
		boost::unordered_map<String, int>::iterator mit = el_map.begin();
		
		// find all unique identifiers:
		for (; mit != el_map.end(); mit++)
		{
			if(mit->second == 1)
				unique_names.push_back(mit->first);
		}
		
		/// a not yet captured case, but examples for this are probably very rare,
		/// because the connection to the partner is excluded, thus for all carbons
		/// we either have 3 identical bonds or at least one bond that is unique.
		if(unique_names.size() < 1)
		{
			cout<<"ERROR: currently we can't handle cases where different but"<<endl;
			cout<<" not unique bonds to a central atom exist"<<endl<<endl;
			exit(EXIT_FAILURE);
		}
		
		/// get all unique atoms (element+order) and positions 
		/// (for the matches in the template)
		for(int j = 0; j < unique_names.size(); j++)
		{
			for (int i = 1; i < site.size(); i++)
			{
				if ( getBondName(site[i], site[0]) == unique_names[j])
				{
					unique_atm.push_back(site[i]);
					unique_pos.push_back(i);
				}
			}
		}
		
		// Find mapping using the found unique atoms.
		// Take the first two, get matrix and return early:
		if(unique_atm.size() >= 2)
		{
			frag2 = unique_atm[0]->getPosition();
			frag3 = unique_atm[1]->getPosition();
			
			tem2 = templ->getAtom(unique_pos[0])->getPosition();
			tem3 = templ->getAtom(unique_pos[1])->getPosition();
			
			return StructureMapper::matchPoints(frag1, frag2, frag3, tem1, tem2, tem3);
		}
		// only one unique does exist, set it and continue with the last step
		else
		{
			frag2 = unique_atm[0]->getPosition();
			tem2 = templ->getAtom(unique_pos[0])->getPosition();
		}
	}
	
	/// all atoms have same element and order:
	/// this means we may choose one match arbitrarily and then test the 
	/// assignment for a second one iteratively
	/// OR: it might be that in the previous step we found already a single unique
	/// atom mapping
	if( frag2.isZero() ) // if frag2 was not yet initialized, arbitrarily choose
											 // the two second atoms to as matching pair
	{
		frag2 = site[1]->getPosition();
		tem2  = templ->getAtom(1)->getPosition();
	}
	
	frag3 = site[2]->getPosition();
	
	/// test all remaining assignments:
	// create test molecules:
	Molecule* dummy_frag;
	Molecule ref_frag;
	for(int i = 0; i < site.size(); i++)
		ref_frag.insert( *(new Atom (*site[i])) );

	dummy_frag = new Molecule(ref_frag, true);
	
	// assignment search loop (because a canonical ordering is not always possible for star-graphs)
	AtomIterator ati = templ->beginAtom();
	ati++; // we used first atom already
	for(; ati != templ->endAtom(); ati++ )
	{
		tem3 = ati->getPosition();
		
		if(tem2 == tem3) // it's possible that our unique assignment did set these coordinates already for tmp2
			continue;      // in that case cycle!
		
		result = StructureMapper::matchPoints(frag1, frag2, frag3, tem1, tem2, tem3);
		TransformationProcessor transformer(result);
		dummy_frag->apply(transformer);
		
		if ( allMatch(dummy_frag, templ) )
		{
			delete dummy_frag;
			return result;
		}
		else
		{
			delete dummy_frag;
			dummy_frag = new Molecule(ref_frag, true);
		}
	}
	
	cout<<"ERROR: could not match"<<endl;
	exit(EXIT_FAILURE);
	return result;
}


/// Handle connections where at least one fragment contains only one atom
void handleSimpleConnections( Atom* atm1, Atom* atm2,
															boost::unordered_map <String, float >& bondLib,
															boost::unordered_map <String, Molecule* >& connectLib)
{
	Molecule* frag1 = (Molecule*)atm1->getParent();
	Molecule* frag2 = (Molecule*)atm2->getParent();
	Molecule* single_frag;
	
	Atom* con1=atm1;
	Atom* con2=atm2;
	Bond* bnd = new Bond();
	bnd->setOrder(1);
	
	String bond_key = atm1->getElement().getSymbol();
	bond_key += atm2->getElement().getSymbol();
	
	// both fragments have just one atom
	if( frag1->countAtoms() + frag2->countAtoms() == 2)
	{
		atm1->setPosition(Vector3(0,0,0));
		atm2->setPosition(Vector3(bondLib[bond_key],0,0));

		frag1->insert(*atm2);
		return; // return early
	}
	// frag 1 is single
	else if(frag1->countAtoms() == 1)
	{
		// insert the single atom1 into frag2
		con1 = atm2;
		con2 = atm1;
		single_frag = frag2;
	}
	// frag 2 is single
	else
	{
		// insert the single atom2 into frag1
		con1 = atm1;
		con2 = atm2;
		single_frag = frag1;
	}
	
	vector< Atom* > site; String key;
	
	int pos = getSite(con1, site, con2, key);
	Molecule* templ = new Molecule(*connectLib[key]);
	
	con2->setPosition( templ->getAtom(pos)->getPosition() );
	templ->remove(*templ->getAtom(pos));
	
	// rotate the single_frag so that it aligns with the template:
	Matrix4x4 trans = align(site, templ);
	TransformationProcessor transformer(trans);
	single_frag->apply(transformer);
	
	single_frag->insert(*con2);
	con1->createBond(*bnd, *con2);
}


///  connectFragments, thereby merge connected fragments
///----------------------------------------------------------------------------
void connectFragments(Molecule* mol,
											list< pair<Atom*, Atom*> >& connections,
											boost::unordered_map <String, Molecule* >& connectLib,
											boost::unordered_map <String, float >& bondLib)
{
	list< pair<Atom*, Atom*> >::iterator con_it = connections.begin();
	
	Atom* anchor_atom = 0;
	for(; con_it != connections.end(); con_it++)
	{
		Atom* con1 = (*con_it).first;
		Atom* con2 = (*con_it).second;
		anchor_atom = con1;
		
		Molecule* frag1 = (Molecule*)con1->getParent();
		Molecule* frag2 = (Molecule*)con2->getParent();
		
//		cout<<"Connecting Atoms "<<con1<< " and "<< con2<<endl;
//		cout<<"Connecting Fragments "<<frag1<< " and "<< frag2<<endl;
		
		///0) Check for trivial cases of one being a single atom or both being single atoms
		if(frag1->countAtoms() == 1 || frag2->countAtoms() == 1)
		{
			handleSimpleConnections(con1, con2, bondLib, connectLib);
		}
		
//		cout<<"searching sites..."<<endl<<endl;
		///1) find connection sites from the two atom pointers:
		vector< Atom* > site1, site2;
		String key1, key2;
		int pos1 = getSite(con1, site1, con2, key1);
		int pos2 = getSite(con2, site2, con1, key2);
		
//		cout<<"site key1: "<<key1<<endl;
//		cout<<"site key2: "<<key2<<endl<<endl;
//		cout<<"......done!"<<endl<<endl;
		
//		cout<<"searching templates..."<<endl;
		///2) find the corresponding templates
		// create working_copies to keep the originals save!
		Molecule* templ1 = new Molecule(*connectLib[key1]);
		Molecule* templ2 = new Molecule(*connectLib[key2]);
	
//		cout<< "found templates: "<< connectLib[key1] <<" "<<templ1->countAtoms() <<" - "<<connectLib[key2]<<" "<<templ2->countAtoms()<<endl;
//		cout<<"......done!"<<endl<<endl;
		
//		cout<<"creating connected template..."<<endl;
		///3) connect the two templates to one new template
		mergeTemplates(templ1, pos1, templ2, pos2, bondLib);
//		cout<<"......done!"<<endl<<endl;
		
//		cout<<"calculating transformations for fragments to the connected template..."<<endl;
		///4) rotate both sites to match the template
		Matrix4x4 trans1 = align(site1, templ1);
		Matrix4x4 trans2 = align(site2, templ2);
//		cout<<"......done!"<<endl<<endl;
		
//		cout<<"applying the transformations to both fragments..."<<endl;
		TransformationProcessor transformer;
		transformer.setTransformation(trans1);
		frag1->apply(transformer);
		
		transformer.setTransformation(trans2);
		frag2->apply(transformer);
//		cout<<"......done!"<<endl<<endl;
		
		
//		cout<<"writing correct molecule type..."<<endl;
		///5) finishing molecule connection on data-type level:
		// form new bond:
		Bond* bnd = new Bond;
		bnd->setOrder(1);
		con1->createBond( *bnd,*(con2) );
		
		// transfer atoms to frag1:
		transferMolecule(frag1, frag2);
//		cout<<"......done!"<<endl<<endl;
		
		///6) clean up:
		delete templ1;
		delete templ2;
	}
	
	// move atoms from the last fragment back to 'mol'
	transferMolecule(mol, (Molecule*)anchor_atom->getParent() );
}


#endif // ASSEMBLER_H