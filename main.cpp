#include "boost/geometry.hpp"
#include "boost/geometry/index/rtree.hpp"
#include "boost/geometry/geometries//point.hpp"
#include <boost/container/static_vector.hpp>

#include <vector>
#include <random>
#include <cmath>
#include <fstream>

constexpr double pi = 3.14159265359;
constexpr size_t num_particles = 10000;
constexpr double particle_diameter = 1.;
constexpr double touching_tolerance = 1e-4;
constexpr double spawn_distance = 1e10;

namespace bg = boost::geometry;
using point = bg::model::point<double, 2, bg::cs::cartesian>;

using rtree = bg::index::rtree<point, bg::index::quadratic<16>>;

struct random_source{
	std::random_device dev;
	std::mt19937 generator;
	random_source() : generator(dev()) {}

	double operator()(double lower, double upper){
		std::uniform_real_distribution<> dist(lower, upper);
		return dist(generator);
	}
};

point brownian_step(point pt, double distance, random_source& random){
	double angle = random(0, 2*pi);
	return point(std::sin(angle) * distance + pt.get<0>(), std::cos(angle) * distance + pt.get<1>());
}

point attach_point(rtree const& points, double spawn_distance, double particle_size, double tolerance, random_source& random){
	point new_particle = brownian_step(point{0, 0}, spawn_distance, random);
	//int i = 0;
	while(true){
		//std::cout << new_particle.get<0>() << ' ' << new_particle.get<1>() << '\n';
		boost::container::static_vector<point, 1> output;
		bg::index::query(points, bg::index::nearest(new_particle, 1), std::back_inserter(output));
		point nearest_point = output.front();
		double step_size = bg::distance(new_particle, nearest_point) - particle_size + tolerance;
		point next_point = brownian_step(new_particle, step_size, random);
		if(bg::distance(next_point, nearest_point) <= particle_size + tolerance){
			return next_point;
		}
		else if(bg::distance(next_point, point{0, 0}) > spawn_distance * 1e3){
			// respawn point when drifting too far to prevent numerical overflow
			new_particle = brownian_step(point{0, 0}, spawn_distance, random);
		}
		else{
			new_particle = next_point;
		}
	}
	//std::cout << points.size() << std::endl;
	//exit(1);
}

int main(){
	random_source random;

	rtree points;
	points.insert(point{0, 0});

	for(size_t i=0; i<num_particles; i++){
		point new_point = attach_point(points, spawn_distance, particle_diameter, touching_tolerance, random);
		points.insert(new_point);
	}

	std::ofstream output;
	output.open("output");
	for(point const& pt : points){
		output << pt.get<0>() << ' ' << pt.get<1>() << '\n';
	}
}
