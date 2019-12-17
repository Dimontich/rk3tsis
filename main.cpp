#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>

std::mt19937 gen{std::random_device{}()};

using Vector = std::vector<double>;

struct Coordinate
{
    double x;
    double y;
    
    Coordinate operator+(const Coordinate &right)
    {
        return {this->x + right.x,
            this->y + right.y};
    }
    
    Coordinate operator-(const Coordinate &right)
    {
        return {this->x - right.x,
            this->y - right.y};
    }
    
    void cut()
    {
        x = (x > 1.) ? 1. : x;
        y = (y > 1.) ? 1. : y;
        x = (x < -1.) ? -1. : x;
        y = (y < -1.) ? -1. : y;
    }
};

double function(const Coordinate &c)
{
    return -std::cos(c.x) * std::cos(c.y) * std::log(1 + c.x * c.x + c.y * c.y);
}

std::vector<Coordinate> bestPoints;

class Population
{
    std::vector<Coordinate> coordinates{};
    
public:
    Population()
    {
        static std::uniform_real_distribution<double> f{-1, 1};
        
        coordinates = {
            {f(gen), f(gen)},
            {f(gen), f(gen)},
            {f(gen), f(gen)},
            {f(gen), f(gen)},
        };
        
        std::cout << "|    1 (x)   |    1 (y)   |    f (1)   |    2 (x)   |    2 (y)   |    f (2)   "
        << "|    3 (x)   |    3 (y)   |    f (3)   |    4 (x)   |    4 (y)   |    f (4)   |\n"
        << "+------------+------------+------------+------------+------------+------------"
        << "+------------+------------+------------+------------+------------+------------+\n";
        
        
        for (const Coordinate &coord: coordinates) {
            std::cout << "| "
            << std::setw(10) << std::setprecision(6) << coord.x
            << " | "
            << std::setw(10) << std::setprecision(6) << coord.y
            << " | "
            << std::setw(10) << std::setprecision(6) << function(coord)
            << " ";
        }
        std::cout << "| Constructor\n+------------+------------+------------+------------+------------+------------"
        << "+------------+------------+------------+------------+------------+------------+\n";
        
        saveBestPoint();
        printInfo();
    }
    
    Coordinate select(const Vector &results, double cur)
    {
        double curRes = 0;
        size_t index = 0;
        for (double next: results) {
            if (curRes <= cur && cur < curRes + next) {
                break;
            }
            index++;
            curRes += next;
        }
        if (index == results.size()) --index;
        
        return coordinates[index];
    }
    
    void go(size_t number)
    {
        // селекция (рулеточная)
        Vector results;
        results.resize(coordinates.size());
        std::transform(coordinates.begin(), coordinates.end(), results.begin(), function);
        
        double sum = std::accumulate(results.begin(), results.end(), 0.);
        std::transform(results.begin(), results.end(),
                       results.begin(), [sum](double result) { return 1 / (result / sum); });
        
        static std::uniform_real_distribution<double> f{0, 1};
        
        std::vector<Coordinate> newCoords;
        for (size_t i = 0; i < 2; i++) {
            double cur = f(gen);                    // ген шанса
            newCoords.push_back(select(results, cur));  // выбор особи
        }
        
        for (const Coordinate &coord: newCoords) {
            std::cout << "| "
            << std::setw(10) << std::setprecision(6) << coord.x
            << " | "
            << std::setw(10) << std::setprecision(6) << coord.y
            << " | "
            << std::setw(10) << std::setprecision(6) << function(coord)
            << " ";
        }
        std::cout << "|            |            |            |            |            |            "
        << "| N = " << number << " (select)\n"
        << "+------------+------------+------------+------------+------------+------------"
        << "+------------+------------+------------+------------+------------+------------+\n";
        
        // кроссинг аут
        coordinates = {
            {newCoords[0].x, newCoords[0].y},
            {newCoords[0].x, newCoords[1].y},
            {newCoords[1].x, newCoords[0].y},
            {newCoords[1].x, newCoords[1].y},
        };
        for (const Coordinate &coord: coordinates) {
            std::cout << "| "
            << std::setw(10) << std::setprecision(6) << coord.x
            << " | "
            << std::setw(10) << std::setprecision(6) << coord.y
            << " | "
            << std::setw(10) << std::setprecision(6) << function(coord)
            << " ";
        }
        std::cout << "| N = " << number << " (crossinhout)\n"
        << "+------------+------------+------------+------------+------------+------------"
        << "+------------+------------+------------+------------+------------+------------+\n";
        
        // мутация
        for (Coordinate &coord: coordinates) {
            static std::uniform_int_distribution<int> f{0, 100};
            if (f(gen) < 25) {
                static std::uniform_real_distribution<double> f{-1, 1};
                coord = coord + Coordinate{f(gen), f(gen)};
            }
        }
        for (const Coordinate &coord: coordinates) {
            std::cout << "| "
            << std::setw(10) << std::setprecision(6) << coord.x
            << " | "
            << std::setw(10) << std::setprecision(6) << coord.y
            << " | "
            << std::setw(10) << std::setprecision(6) << function(coord)
            << " ";
        }
        std::cout << "| N = " << number << " (mutator)\n"
        << "+------------+------------+------------+------------+------------+------------"
        << "+------------+------------+------------+------------+------------+------------+\n";
        
        printInfo();
        
        saveBestPoint();
    }
    
    void printInfo()
    {
        Coordinate &best = coordinates[0];
        double bestResult = function(coordinates[0]);
        double avg = 0;
        
        for (Coordinate &c: coordinates) {
            double newResult = function(c);
            avg += newResult;
            if (function(c) > bestResult) {
                best = c;
                bestResult = newResult;
            }
        }
        
        std::cout << "|   Best fit: "
        << std::setw(10) << std::setprecision(6) << bestResult << "  |   "
        << "AVG fit: "
        << std::setw(10) << std::setprecision(6) << avg / coordinates.size() << "   |\n"
        << "+------------+------------+------------+------------+------------+------------"
        << "+------------+------------+------------+------------+------------+------------+\n";
    }
    
    void saveBestPoint()
    {
        Coordinate &best = coordinates[0];
        double bestResult = function(coordinates[0]);
        
        for (Coordinate &c: coordinates) {
            double newResult = function(c);
            if (function(c) > bestResult) {
                best = c;
                bestResult = newResult;
            }
        }
        
        bestPoints.push_back(best);
    }
};

int main()
{
    Population population;
    
    for (size_t i = 1; i <= 10; ++i) {
        population.go(i);
    }
    
    std::cout << "\n\nBest point: " << bestPoints.back().x << " " << bestPoints.back().y;
    std::cout << "\n\nCopy (yotx.ru):\n  ";
    for (const Coordinate &point: bestPoints) {
        std::cout << "(" << point.x << ";"  << point.y << ") ";
    }
    
    return 0;
}

