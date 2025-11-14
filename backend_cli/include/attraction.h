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
    int popularity;
    std::vector<std::string> tags;
    std::string description;
    std::string openingHours;

    Attraction()
        : id(-1), latitude(0), longitude(0),
          visitDuration(0), rating(0), entryFee(0), popularity(0) {}
};

#endif // ATTRACTION_H
