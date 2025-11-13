#ifndef ATTRACTION_H
#define ATTRACTION_H

#include <string>
#include <vector>

struct Attraction {
    int id;
    std::string name;
    std::string category;
    double latitude;
    double longitude;
    double visitDuration;
    double rating;
    double entryFee;
    std::vector<std::string> tags;
    int popularity;
    std::string description;
    std::string openingHours;

    Attraction() : id(0), latitude(0), longitude(0), visitDuration(0),
                   rating(0), entryFee(0), popularity(0) {}

    Attraction(int _id, std::string _name, std::string _category,
               double _lat, double _lon, double _duration, double _rating,
               double _fee, int _pop)
        : id(_id), name(_name), category(_category), latitude(_lat),
          longitude(_lon), visitDuration(_duration), rating(_rating),
          entryFee(_fee), popularity(_pop) {}
};

#endif
