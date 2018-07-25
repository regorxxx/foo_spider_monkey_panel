#include "stdafx.h"
#include "kmeans.h"


/*
k-means is a clustering algorithm designed to group data points into clusters of similar points,
and return the averaged "center" value of each cluster. The algorithm runs over the data set
multiple times, assigning points to the nearest center, and then re-calculating the center values
after each iteration, until the centers stabilize of max_iterations have been run.

Detailed information here: https://en.wikipedia.org/wiki/K-means_clustering

Here it is being used to group RGB colour values into clusters of similar colours for the purposes
of generating a colour scheme from an image. Each data point is a distinct RGB value that
represents a number of pixels with the same RGB value from the original image. Therefore while
every data-point is distinct, they do not all carry the same "weight" for the purposes of
determining the center points of each cluster.

In standard k-means, the starting center values are chosen at random. This provides better results
at the expense of potentially different values on subsequent runs with the same inputs. That was
not acceptable for generating colour values, so the starting center colour values are evenly spaced
across the data set.
*/

namespace
{
uint8_t kNumberOfColourComponents = 3;
}

namespace kmeans
{

Point::Point( int id_point, const std::vector<uint32_t>& values, uint32_t pixel_count )
{
    this->id_point = id_point;
    this->pixel_count = pixel_count;
    this->values = values; 

    id_cluster = -1;
}

int Point::getID() const
{
    return id_point;
}

void Point::setCluster( int id_cluster )
{
    this->id_cluster = id_cluster;
}

int Point::getCluster()const
{
    return id_cluster;
}

uint32_t Point::getValue( int index )const
{
    return values[index];
}

uint32_t Point::getTotalValues() const
{
    return total_values;
}

uint32_t Point::getPixelCount() const
{
    return pixel_count;
}


Cluster::Cluster( int id_cluster, Point point )
{
    this->id_cluster = id_cluster;

    uint32_t total_values = point.getTotalValues();

    for ( uint32_t i = 0; i < total_values; i++ )
        central_values.push_back( point.getValue( i ) );

    points.push_back( point );
}

void Cluster::addPoint( Point point )
{
    points.push_back( point );
}

bool Cluster::removePoint( int id_point )
{
    uint32_t total_points = points.size();

    for ( uint32_t i = 0; i < total_points; i++ )
    {
        if ( points[i].getID() == id_point )
        {
            points.erase( points.begin() + i );
            return true;
        }
    }
    return false;
}

double Cluster::getCentralValue( int index ) const
{
    return central_values[index];
}

void Cluster::setCentralValue( int index, double value )
{
    central_values[index] = value;
}

Point Cluster::getPoint( int index ) const
{
    return points[index];
}

int Cluster::getTotalPoints() const
{
    int total = 0;
    for ( uint32_t i = 0; i < points.size(); i++ )
        total += points[i].getPixelCount();

    return total;
}

int Cluster::getSize() const
{
    return points.size();
}

// return ID of nearest center
// uses distance calculations from: https://en.wikipedia.org/wiki/Color_difference
int KMeans::getIDNearestCenter( Point point ) const
{
    double sum = 0.0, min_dist;
    int id_cluster_center = 0;

    sum += 2 * pow( clusters[0].getCentralValue( 0 ) - point.getValue( 0 ), 2.0 ); // r
    sum += 4 * pow( clusters[0].getCentralValue( 1 ) - point.getValue( 1 ), 2.0 ); // g
    sum += 3 * pow( clusters[0].getCentralValue( 2 ) - point.getValue( 2 ), 2.0 ); // b

    min_dist = sum;

    for ( int i = 1; i < K; i++ )
    {
        double dist;
        sum = 0.0;

        sum += 2 * pow( clusters[i].getCentralValue( 0 ) - point.getValue( 0 ), 2.0 );
        sum += 4 * pow( clusters[i].getCentralValue( 1 ) - point.getValue( 1 ), 2.0 );
        sum += 3 * pow( clusters[i].getCentralValue( 2 ) - point.getValue( 2 ), 2.0 );

        dist = sum;

        if ( dist < min_dist )
        {
            min_dist = dist;
            id_cluster_center = i;
        }
    }

    return id_cluster_center;
}

KMeans::KMeans( int K, int total_points, int max_iterations )
{
    this->K = std::min( std::max( K, 14 ), total_points );
    this->total_points = total_points;
    this->colour_components = kNumberOfColourComponents;
    this->max_iterations = max_iterations;
}

std::vector<Cluster> KMeans::run( std::vector<Point> & points )
{
    std::vector<int> prohibited_indexes;

    // choose K distinct values for the centers of the clusters
    int index_point = 0;
    for ( int i = 0; i < K; i++ )
    {
        index_point = (int)(i * total_points / K); // colours are already distinct so we can't have duplicate centers
        points[index_point].setCluster( i );
        Cluster cluster( i, points[index_point] );
        clusters.push_back( cluster );
    }

    int iter = 1;

    while ( true )
    {
        bool done = true;

        // associate each point to its nearest center
        for ( int i = 0; i < total_points; i++ )
        {
            int id_old_cluster = points[i].getCluster();
            int id_nearest_center = getIDNearestCenter( points[i] );

            if ( id_old_cluster != id_nearest_center )
            {
                if ( id_old_cluster != -1 )
                {
                    clusters[id_old_cluster].removePoint( points[i].getID() );
                }

                points[i].setCluster( id_nearest_center );
                clusters[id_nearest_center].addPoint( points[i] );
                done = false;
            }
        }

        // recalculating the center of each cluster
        for ( int i = 0; i < K; i++ )
        {
            for ( int j = 0; j < colour_components; j++ )
            {
                int total_points_cluster = clusters[i].getTotalPoints();
                double sum = 0.0;

                if ( total_points_cluster > 0 )
                {
                    for ( int p = 0; p < clusters[i].getSize(); p++ )
                        sum += clusters[i].getPoint( p ).getValue( j ) * clusters[i].getPoint( p ).getPixelCount();
                    clusters[i].setCentralValue( j, sum / total_points_cluster );
                }
            }
        }

        if ( done == true || iter >= max_iterations )
        {
            break;
        }

        iter++;
    }

    return clusters;
}

}