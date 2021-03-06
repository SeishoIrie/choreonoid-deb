/**
   \file
   \brief The header file of the LinkTraverse class
   \author Shin'ichiro Nakaoka
*/

#ifndef CNOID_BODY_LINK_TRAVERSE_H_INCLUDED
#define CNOID_BODY_LINK_TRAVERSE_H_INCLUDED

#include <vector>
#include <ostream>
#include "exportdecl.h"

namespace cnoid {

    class Link;

    class CNOID_EXPORT LinkTraverse
    {
      public:
        LinkTraverse();
        LinkTraverse(int size);
        LinkTraverse(Link* root, bool doUpward = false, bool doDownward = true);

        virtual ~LinkTraverse();

        virtual void find(Link* root, bool doUpward = false, bool doDownward = true);

        inline int numLinks() const {
            return links.size();
        }

        inline bool empty() const {
            return links.empty();
        }

        inline size_t size() const {
            return links.size();
        }

        inline Link* rootLink() const {
            return (links.empty() ? 0 : links.front());
        }

        inline Link* link(int index) const {
            return links[index];
        }

        inline Link* operator[] (int index) const {
            return links[index];
        }

        inline std::vector<Link*>::const_iterator begin() const {
            return links.begin();
        }

        inline std::vector<Link*>::const_iterator end() const {
            return links.end();
        }
	
        /**
           If the connection from the queried link to the next link is downward (forward) direction,
           the method returns true. Otherwise, returns false.
           The range of valid indices is 0 to (numLinks() - 2).
        */
        inline bool isDownward(int index) const {
            return (index >= numUpwardConnections);
        }
	
        void calcForwardKinematics(bool calcVelocity = false, bool calcAcceleration = false) const;

      protected:
        
        std::vector<Link*> links;
        int numUpwardConnections;

      private:
        
        void traverse(Link* link, bool doUpward, bool doDownward, bool isUpward, Link* prev);

    };

};

CNOID_EXPORT std::ostream& operator<<(std::ostream& os, cnoid::LinkTraverse& traverse);

#endif
