//
// Created by lucas on 04/02/24.
//

#ifndef MMV_ASTAR_H
#define MMV_ASTAR_H

#include <list>
#include <algorithm>

#include "../gKit/wavefront.h"
#include "../gKit/texture.h"

#include "ScalarField.h"

class map {
public:
    map() {
        std::array<int,64> t = {
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 1, 1, 1, 0,
                0, 0, 1, 0, 0, 0, 1, 0,
                0, 0, 1, 0, 0, 0, 1, 0,
                0, 0, 1, 1, 1, 1, 1, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0
        };
        w = h = 8;
        m.resize(w * h);
        for( int r = 0; r < w * h; r++ )
            m[r] = t[r];
    }

    map(int width, int height) {
        w = width;
        h = height;
        m.resize(w * h);
        for( int r = 0; r < w * h; r++ )
            //rand() % 10 > 7 ? m[r] = 1 :
            m[r] = 0;
    }

    int operator() ( int x, int y ) { return m[y*w + x]; }
    std::vector<int> m;
    int w, h;
};

class node {
public:
    bool operator == (const node& o ) { return pos == o.pos; }
    bool operator == (const vec2& o ) { return pos == o; }
    bool operator < (const node& o ) { return dist + cost < o.dist + o.cost; }
    vec2 pos, parent;
    int dist, cost;
};


class aStar {
public:
    aStar() {
        neighbours[0] = vec2( -1, -1 ); neighbours[1] = vec2(  1, -1 );
        neighbours[2] = vec2( -1,  1 ); neighbours[3] = vec2(  1,  1 );
        neighbours[4] = vec2(  0, -1 ); neighbours[5] = vec2( -1,  0 );
        neighbours[6] = vec2(  0,  1 ); neighbours[7] = vec2(  1,  0 );
    }

    int calcDist( vec2& p ){
        // Euclidian Distance
        //int x = end.x - p.x, y = end.y - p.y;
        //return( x * x + y * y );

        // Manhattan Distance
        return abs(end.x - p.x) + abs(end.y - p.y);
    }

    bool isValid( vec2& p ) {
        return ( p.x >-1 && p.y > -1 && p.x < m.w && p.y < m.h );
    }

    bool existPoint( vec2& p, int cost ) {
        std::list<node>::iterator i;
        i = std::find( closed.begin(), closed.end(), p );
        if( i != closed.end() ) {
            if( ( *i ).cost + ( *i ).dist < cost ) return true;
            else { closed.erase( i ); return false; }
        }
        i = std::find( open.begin(), open.end(), p );
        if( i != open.end() ) {
            if( ( *i ).cost + ( *i ).dist < cost ) return true;
            else { open.erase( i ); return false; }
        }
        return false;
    }

    bool fillOpen( node& n ) {
        int stepCost, nc, dist;
        vec2 neighbour;

        for( int x = 0; x < 8; x++ ) {
            // one can make diagonals have different cost
            stepCost = x < 4 ? 1 : 1;
            neighbour = n.pos + neighbours[x];
            if( neighbour == end ) return true;

            if( isValid( neighbour ) && m( neighbour.x, neighbour.y ) != 1 ) {
                nc = stepCost + n.cost;
                dist = calcDist( neighbour );
                if( !existPoint( neighbour, nc + dist ) ) {
                    node m;
                    m.cost = nc; m.dist = dist;
                    m.pos = neighbour;
                    m.parent = n.pos;
                    open.push_back( m );
                }
            }
        }
        return false;
    }

    bool search( vec2& s, vec2& e, map& mp ) {
        node n; end = e; start = s; m = mp;
        n.cost = 0; n.pos = s; n.parent = 0; n.dist = calcDist( s );
        open.push_back( n );
        while( !open.empty() ) {
            //open.sort();
            node n = open.front();
            open.pop_front();
            closed.push_back( n );
            if( fillOpen( n ) ) return true;
        }
        return false;
    }

    int path( std::list<vec2>& path ) {
        path.push_front( end );
        int cost = 1 + closed.back().cost;
        path.push_front( closed.back().pos );
        vec2 parent = closed.back().parent;

        for( auto i = closed.rbegin(); i != closed.rend(); i++ ) {
            if( ( *i ).pos == parent && !( ( *i ).pos == start ) ) {
                path.push_front( ( *i ).pos );
                parent = ( *i ).parent;
            }
        }
        path.push_front( start );
        return cost;
    }

    void makeRoad(vec2 start, vec2 end, map& m, Image& roads) {
        if(search( start, end, m ) ) {
            int mapWidth = roads.width();
            int mapHeight = roads.height();
            std::list<vec2> path;
            this->path(path);
            for( int y = -1; y < mapHeight; y++ ) {
                for( int x = -1; x < mapWidth; x++ ) {
                    if( std::find( path.begin(), path.end(), vec2( x, y ) )!= path.end() ) {
                        roads(x, y) = Color(1, 1, 1);
                    }
                }
            }
            roads(start.x, start.y) = Color(1, 0, 0);
            roads(end.x, end.y) = Color(1, 0, 0);
            write_image(roads, "roads.png");
        }
    }

    map m; vec2 end, start;
    vec2 neighbours[8];
    std::list<node> open;
    std::list<node> closed;
};

#endif //MMV_ASTAR_H
