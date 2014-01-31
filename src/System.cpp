#include <nbody/constants.h>
#include <nbody/System.h>
#include <nbody/Vector3.h>

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>

namespace nbody {
    
    // writes out acceleration into acc (after interacting bodies i, j)
    inline void System::interactBodies(size_t i, size_t j, float softFactor, Vector3f &acc) const {
        Vector3f r = _body[j].position() - _body[i].position();
        float distance = r.norm() + softFactor;
        float invDist = 1.0f / distance;
        float invDistCubed = cube(invDist);
        acc = acc + NEWTON_G * _body[j].mass() * invDistCubed * r;
    }
    
    // interact each pair of bodies (mutates accelerations)
    void System::computeGravitation() {
        for(size_t i = 0; i < _nBodies; ++i) {
            Vector3f acc{ 0.0f, 0.0f, 0.0f };
            for(size_t j = 0; j < _nBodies; ++j) {
                if(i != j) {
                    interactBodies(i, j, _softFactor, acc);
                }
            }
            _body[i].force() = acc;
        }
    }
    
    // actual integration
    void System::integrateSystem(float dt) {
        Vector3f r, v, a;
        for(size_t i = 0; i < _nBodies; ++i) {
            r = _body[i].position();
            v = _body[i].velocity();
            a = _body[i].force();
            
            v = v + (a * dt);
            v = v * _dampingFactor;
            r = r + v * dt;
            
            _body[i].position() = r;
            _body[i].velocity() = v;
        }
    }
    
    void System::update(float dt) {
        computeGravitation();
        integrateSystem(dt);
    }
    
    void System::readState(std::istream &input) {
        input >> _nBodies;
        if(_nBodies > MAX_BODIES_RECOMMENDED) {
            throw std::runtime_error("Too many input bodies");
        }
        _body = new Body[_nBodies];
        for(size_t i = 0; i < _nBodies; ++i) {
            input >> _body[i];
        }
    }
    
    void System::writeState(std::ostream &output) const {
        output << _nBodies << "\n";
        for(size_t i = 0; i < _nBodies; ++i) {
            output << _body[i] << "\n";
        }
    }

    void System::returnState(std::ostream &output) const {
        output << _nBodies << "\n";
        for(size_t i = 0; i < _nBodies; ++i) {
            output << _body[i] << "\n";
        }
    }

    // get the new coordinates (x1, y1, z1, x2, y2, z2)
    float* System::getNewCoords() {
        float *values = new float[_nBodies * 3]; // remember to delete []!!
        for(size_t i = 0; i < _nBodies; ++i) {
            values[3*i] = _body->position().x();
            values[3*i+1] = _body->position().y();
            values[3*i+2] = _body->position().z();
        }

        return values;
    }
    
     // gets the number of bodies
    size_t System::getNbodies() {
        return _nBodies;
    }
    
} // namespace nbody
