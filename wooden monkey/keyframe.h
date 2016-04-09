/***********************************************************
             CSC418, FALL 2009
 
                 keyframe.h
                 author: Mike Pratscher

		Keyframe class
			This class provides a data structure that
			represents a keyframe: (t_i, q_i)
			where t_i is the time and
			      q_i is the pose vector at that time
			(see vector.h file for info on Vector class).
			The data structure also includes an ID to
			identify the keyframe.

***********************************************************/

#ifndef __KEYFRAME_H__
#define __KEYFRAME_H__

#include "vector.h"


class Keyframe
{
public:

	// Enumeration describing the supported joint DOFs.
	// Use these to access DOF values using the getDOF() function
	// NOTE: If you decide to add more DOFs, be sure to add the
	// corresponding enums _BEFORE_ the NUM_JOINT_ENUM element
	//
	enum { ROOT_TRANSLATE_X, ROOT_TRANSLATE_Y, ROOT_TRANSLATE_Z,
		   ROOT_ROTATE_X,    ROOT_ROTATE_Y,    ROOT_ROTATE_Z,
		   
		   PLAYER_TRANSLATE_X, PLAYER_TRANSLATE_Y, PLAYER_TRANSLATE_Z,
		   PLAYER_ROTATE_X,    PLAYER_ROTATE_Y,    PLAYER_ROTATE_Z,

		   E1_TRANSLATE_X, E1_TRANSLATE_Y, E1_TRANSLATE_Z,
		   E1_ROTATE_X,    E1_ROTATE_Y,    E1_ROTATE_Z,
		   
		   E2_TRANSLATE_X, E2_TRANSLATE_Y, E2_TRANSLATE_Z,
		   E2_ROTATE_X,    E2_ROTATE_Y,    E2_ROTATE_Z,
		   
		   E3_TRANSLATE_X, E3_TRANSLATE_Y, E3_TRANSLATE_Z,
		   E3_ROTATE_X,    E3_ROTATE_Y,    E3_ROTATE_Z,
		   
		   EnemyPlane_TRANSLATE_X, EnemyPlane_TRANSLATE_Y, EnemyPlane_TRANSLATE_Z,
		   EnemyPlane_ROTATE_X,    EnemyPlane_ROTATE_Y,    EnemyPlane_ROTATE_Z,
		   EnemyPlane_PROPELLER,
		   
		   SHELL1_TRANSLATE_X, SHELL1_TRANSLATE_Y, SHELL1_TRANSLATE_Z,
		   SHELL1_ROTATE_X,    SHELL1_ROTATE_Y,    SHELL1_ROTATE_Z,
		   
		   SHELL2_TRANSLATE_X, SHELL2_TRANSLATE_Y, SHELL2_TRANSLATE_Z,
		   SHELL2_ROTATE_X,    SHELL2_ROTATE_Y,    SHELL2_ROTATE_Z,

		   SHELL3_TRANSLATE_X, SHELL3_TRANSLATE_Y, SHELL3_TRANSLATE_Z,
		   SHELL3_ROTATE_X,    SHELL3_ROTATE_Y,    SHELL3_ROTATE_Z,

		   SHELL4_TRANSLATE_X, SHELL4_TRANSLATE_Y, SHELL4_TRANSLATE_Z,
		   SHELL4_ROTATE_X,    SHELL4_ROTATE_Y,    SHELL4_ROTATE_Z,

		   SMALLSHELL_TRANSLATE_X, SMALLSHELL_TRANSLATE_Y, SMALLSHELL_TRANSLATE_Z,
		   SMALLSHELL_ROTATE_X,    SMALLSHELL_ROTATE_Y,    SMALLSHELL_ROTATE_Z,
		   
		   SPARK_TRANSLATE_X, SPARK_TRANSLATE_Y, SPARK_TRANSLATE_Z,
		   SPARK_ROTATE_X,    SPARK_ROTATE_Y,    SPARK_ROTATE_Z,
		   SPARK_FRAME, SPARK_SCALE, SPARK_LIGHT,
		   
		   NUM_JOINT_ENUM};

	// constructor
	Keyframe() : id(0), time(0.0), jointDOFS(NUM_JOINT_ENUM) {}

	// destructor
	virtual ~Keyframe() {}

	// accessor methods
	int  getID() const { return id; }
	void setID(int i)  { id = i;    }

	float getTime() const  { return time; }
	void  setTime(float t) { time = t;    }

	// Use enumeration values to specify desired DOF
	float getDOF(int eDOF) const      { return jointDOFS[eDOF]; }
	void  setDOF(int eDOF, float val) { jointDOFS[eDOF] = val;  }

	// These allow the entire pose vector to be obtained / set.
	// Useful when calculating interpolated poses.
	// (see vector.h file for info on Vector class)
	Vector getDOFVector() const            { return jointDOFS;  }
	void   setDOFVector(const Vector& vec) { jointDOFS = vec;   }

	// address accessor methods
	int*   getIDPtr()          { return &id;              }
	float* getTimePtr()        { return &time;            }
	float* getDOFPtr(int eDOF) { return &jointDOFS[eDOF]; }

private:

	int    id;
	float  time;
	Vector jointDOFS;
};

#endif // __KEYFRAME_H__
