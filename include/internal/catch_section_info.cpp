/*
 *  Created by Phil Nash on 4/5/2012
 *  Copyright 2012 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "catch_section_info.h"

#include <vector>

namespace Catch {

    class RunningSection {
    public:

        typedef std::vector<RunningSection*> SubSections;

        enum State {
            Root,
            Unknown,
            Branch,
            TestedBranch,
            TestedLeaf
        };

        RunningSection( RunningSection* parent, std::string const& name )
        :   m_state( Unknown ),
            m_parent( parent ),
            m_name( name )
        {}

        RunningSection( std::string const& name )
        :   m_state( Root ),
            m_parent( nullptr ),
            m_name( name )
        {}

        ~RunningSection() {
            deleteAll( m_subSections );
        }

        std::string getName() const {
            return m_name;
        }

        bool shouldRun() const {
            return m_state < TestedBranch;
        }

        bool isBranch() const {
            return m_state == Branch;
        }

        const RunningSection* getParent() const {
            return m_parent;
        }

        bool hasUntestedSections() const {
            if( m_state == Unknown )
                return true;
            for( auto subSection : m_subSections )
                if( subSection->hasUntestedSections() )
                    return true;
            return false;
        }

        // Mutable methods:

        RunningSection* getParent() {
            return m_parent;
        }

        RunningSection* findOrAddSubSection( std::string const& name, bool& changed ) {
            for( auto subSection : m_subSections )
                if( subSection->getName() == name )
                    return subSection;
            RunningSection* subSection = new RunningSection( this, name );
            m_subSections.push_back( subSection );
            m_state = Branch;
            changed = true;
            return subSection;
        }

        bool ran() {
            if( m_state >= Branch )
                return false;
            m_state = TestedLeaf;
            return true;
        }

        void ranToCompletion() {
            if( m_state == Branch && !hasUntestedSections() )
                m_state = TestedBranch;
        }

    private:
        State m_state;
        RunningSection* m_parent;
        std::string m_name;
        SubSections m_subSections;
    };
}