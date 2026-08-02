#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <stdexcept>
#include <fstream>
#include <string>
#include <filesystem>

#include "RandomGen.h"

std::vector<int> target_generations = {20, 50, 100, 500};

// Struktura reprezentująca osobnika
struct Individual {
    std::vector<long double> objectives;       // Wartości funkcji celu [f1, f2]
    std::vector<long double> decision_variables; // Zmienne decyzyjne
    int rank = 0;                              // Poziom niedominacji
    double crowding_distance = 0.0;            // Odległość tłumna

    // Konstruktor
    Individual(size_t num_variables = 0)
            : decision_variables(num_variables, 0.0L), objectives(2, 0.0L), rank(0), crowding_distance(0.0) {}
};

// Funkcja testowa ZDT
static inline std::vector<long double> ZDT(const std::vector<long double>& x, int function) {
    if (function==1){
        //double f1 = x[0];
        //double g = 1 + 9 * x[1];
        ////std::cout<<"\nx[0]: "<<x[0]<<"; x[1]: "<<x[1]<<std::endl;
        //if (g <= 0) {
        //    std::cerr << "Error: g <= 0, g = " << g << std::endl;
        //}
        //if (f1 / g < 0) {
        //    std::cerr << "Error in ZDT1: sqrt(f1 / g) is invalid, f1 = " << f1 << ", g = " << g << std::endl;
        //}
        //double h = 1 - std::sqrt(f1 / g);
        //return {f1, g*h};


        size_t m = x.size();
        double f1 = x[0];

        // Oblicz g(x)
        double g = 1.0 + 9.0 * std::accumulate(x.begin() + 1, x.end(), 0.0) / (m - 1);

        // Oblicz h(f1, g)
        double h = 1.0 - std::sqrt(f1 / g);

        return { f1, g * h };

    } if (function==2) {
        //double f1 = x[0];
        //double g = 1 + 9 * x[1];
        //if (g <= 0) {
        //    std::cerr << "Error in ZDT2: g <= 0, g = " << g << std::endl;
        //}
        //double pom = f1 / g;
        //double h = 1.0 - (pom*pom);
        //return {f1, g * h};

        size_t m = x.size();
        double f1 = x[0];

        // Oblicz g(x)
        double g = 1.0 + 9.0 * std::accumulate(x.begin() + 1, x.end(), 0.0) / (m - 1);

        // Oblicz h(f1, g)
        double h = 1.0 - std::pow(f1 / g, 2);

        return { f1, g * h };
    } else if (function == 3) {
        //double f1 = x[0];
        //double g = 1 + 9 * x[1];
        //if (g <= 0) {
        //    std::cerr << "Error in ZDT3: g <= 0, g = " << g << std::endl;
        //}
        //double pom = f1 / g;
        //double h = 1.0 - std::sqrt(pom) - pom * std::sin(10 * M_PI * f1);
        //return {f1, g * h};

        size_t m = x.size();
        double f1 = x[0];

        // Oblicz g(x)
        double g = 1.0 + 9.0 * std::accumulate(x.begin() + 1, x.end(), 0.0) / (m - 1);

        // Oblicz h(f1, g)
        double h = 1.0 - std::sqrt(f1 / g) - (f1 / g) * std::sin(10 * M_PI * f1);

        return { f1, g * h };

    } else if (function == 4) {
        //double f1 = x[0];
        //double g = 1 + 10 * (2 - 1) + std::pow(x[1], 2) - 10 * std::cos(4 * M_PI * x[1]);
        //if (g <= 0) {
        //    std::cerr << "Error in ZDT4: g <= 0, g = " << g << std::endl;
        //}
        //double h = 1 - std::sqrt(f1 / g);
        //return {f1, g * h};

        size_t m = x.size();
        double f1 = x[0];

        // Oblicz g(x)
        double g = 1.0 + 10 * (m - 1) + std::accumulate(x.begin() + 1, x.end(), 0.0, [](double sum, double xi)
            {
                return sum + xi * xi - 10 * std::cos(4 * M_PI * xi);
            });

        // Oblicz h(f1, g)
        double h = 1.0 - std::sqrt(f1 / g);

        return { f1, g * h };

    } else if (function == 6) {
        //double f1 = 1.0 - std::exp(-4.0 * x[0]) * std::pow(std::sin(6.0 * M_PI * x[0]), 6);
        //double g = 1.0 + 9.0 * std::pow(std::accumulate(x.begin() + 1, x.end(), 0.0L) / (x.size() - 1), 0.25);
        //if (g <= 0) {
        //    std::cerr << "Error in ZDT6: g <= 0, g = " << g << std::endl;
        //}
        //double h = 1.0 - std::pow(f1 / g, 2.0);
        //return {f1, g * h};

        size_t m = x.size();
        double f1 = 1.0 - std::exp(-4.0 * x[0]) * std::pow(std::sin(6.0 * M_PI * x[0]), 6);

        // Oblicz g(x)
        double g = 1.0 + 9.0 * std::pow(std::accumulate(x.begin() + 1, x.end(), 0.0) / (m - 1), 0.25);

        // Oblicz h(f1, g)
        double h = 1.0 - std::pow(f1 / g, 2);

        return { f1, g * h };

    } else {
        throw std::invalid_argument("Invalid function number for ZDT.");
    }
}

// Funkcja do inicjalizacji populacji
void initializePopulation(std::vector<Individual>& population, int function) {
    for (auto& ind : population) {
        //ind.decision_variables.resize(2); // m=2 Tutaj byl blad (zmienna ustawiona stale)
        //ind.objectives.resize(2);        // Dwie funkcje celu 

        for (auto& value : ind.decision_variables) {
             value = Random::GetUniform<double>(0.0, 1.0);
        }
        // Oblicz wartości funkcji celu
        ind.objectives = ZDT(ind.decision_variables, function);
//        Debug
//        std::cout << "Decision Variables: ";
//        for (const auto& var : ind.decision_variables) {
//            std::cout << var << " ";
//        }
//        std::cout << "\nObjectives: " << ind.objectives[0] << ", " << ind.objectives[1] << "\n";
    }

}

// Funkcja do zapisu frontu Pareto do pliku
void saveParetoFront(const std::vector<Individual>& front, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for Pareto front: " << filepath << std::endl;
        return;
    }
    for (const auto& ind : front) {
        file << ind.objectives[0] << "," << ind.objectives[1] << "\n";
    }
    file.close();
    //std::cout << "Pareto front saved to: " << filepath << std::endl;
}

// Funkcja do ewaluacji funkcji celu dla populacji
void evaluateObjectives(std::vector<Individual>& population, int function) {
    for (auto& ind : population) {
        ind.objectives = ZDT(ind.decision_variables, function);
    }
}

// Funkcja do sortowania niedominowanego - 184 str NSGA-II
std::vector<std::vector<Individual>> fastNonDominatedSort(std::vector<Individual>& population) {
    std::vector<std::vector<Individual>> fronts;
    std::vector<int> domination_count(population.size(), 0);
    std::vector<std::vector<int>> dominated_solutions(population.size());
    std::vector<int> first_front_indices; // indeksy osobników należacych do pierwszego frontu

    for (size_t i = 0; i < population.size(); ++i) {
        // Dla każdej osoby sprawdzamy dominacje nad innymi osobnikami
        for (size_t j = 0; j < population.size(); ++j) {
            if (i == j) continue; // pomin tego samego osobnika

            // pi > pj <- pi dominates pj
            // pj < pj <- pi is dominated by pj
            bool dominates = true;
            bool dominated_by = true;
            for (size_t k = 0; k < population[i].objectives.size(); ++k) {
                if (population[i].objectives[k] < population[j].objectives[k]) dominated_by = false;
                if (population[i].objectives[k] > population[j].objectives[k]) dominates = false;
            }

            // dla pi dopisujemy osobniki ktore pi dominuje (do kontenera) lub osobniki dominujace pi (jako lista liczników pozostalych osobnikow)
            if (dominates && !dominated_by) {
                dominated_solutions[i].push_back(j);
            } else if (dominated_by && !dominates) {
                domination_count[i]++;
            }
        }

        if (domination_count[i] == 0) {
            population[i].rank = 0;
            first_front_indices.push_back(i);
        }
    }

    //std::vector<int> first_front_indices;
    //for (size_t i = 0; i < population.size(); ++i) {
    //    if (domination_count[i] == 0) {
    //        first_front_indices.push_back(i);
    //    }
    //}

    // stworzenie pierwszego frontu z osobników niezdominowanych
    fronts.push_back({});
    for (int idx : first_front_indices) {
        fronts[0].push_back(population[idx]);
    }


    // tworzenie kolejnych frontow z kontenera zdominowanych osobników
    int current_front = 0;
    while (!fronts[current_front].empty()) {
        std::vector<int> next_front_indices;

        // zmniejsz licznik osobników ktore zdominowaly bierzacego i przenies go do nastepnego frontu jesli licznik jest pusty
        for (int idx : first_front_indices) {
            for (int dom : dominated_solutions[idx]) {
                domination_count[dom]--;
                if (domination_count[dom] == 0) {
                    population[dom].rank = current_front + 1;
                    next_front_indices.push_back(dom);
                }
            }
        }

        // utwórz nowy front z zapisanych indexów osobników należacych do nowego (tego co teraz tworzysz) frontu
        first_front_indices = next_front_indices;
        if (!next_front_indices.empty()) {
            fronts.push_back({});
            for (int idx : next_front_indices) {
                fronts[current_front + 1].push_back(population[idx]);
            }
        }
        else
        {
            break;
        }
        current_front++;  // przejdz do nastepnego frontu
    }

    return fronts;
}

// Funkcja do obliczania odległości tłumnej - 185 str NSGA-II
void calculateCrowdingDistance(std::vector<Individual>& front) {
    size_t num_objectives = front[0].objectives.size(); // liczba ustalonych celów we froncie
    size_t num_individuals = front.size(); // max liczba rezultatów dla kazdego osobnika we froncie

    // inicjalizacja dystansu dla osobników frontu
    for (auto& ind : front) {
        ind.crowding_distance = 0.0;
    }

    // ustalanie odleglosci we froncie
    for (size_t m = 0; m < num_objectives; ++m) {

        // posortowanie osobników tak aby byla mozliwosc wybrania punktów granicznych dla kazdego osobnika (od najmniejszego)
        std::sort(front.begin(), front.end(), [m](const Individual& a, const Individual& b) {
            return a.objectives[m] < b.objectives[m];
        });

        // ustalenie dystansu dla 1szego i ostatniego osobnika
        front[0].crowding_distance = front[num_individuals - 1].crowding_distance = std::numeric_limits<double>::infinity();

        // wartosci funkcyjne fmin i fmax (graniczne)
        double min_obj = front[0].objectives[m];
        double max_obj = front[num_individuals - 1].objectives[m];

        // ustalenie dystansu dla pozostalych osobników
        for (size_t i = 1; i < num_individuals - 1; ++i) {
            front[i].crowding_distance += (front[i + 1].objectives[m] - front[i - 1].objectives[m]) / (max_obj - min_obj);
        }
    }
}

// Funkcja selekcji
std::vector<Individual> selection(const std::vector<Individual>& population) {
    std::vector<Individual> mating_pool;
    //size_t tournament_size = std::max<size_t>(2, population.size() / 10); // Turniej dla ~10% populacji
    int tournament_size = 10; // Stała wartość zamiast zmiennego

    for (size_t i = 0; i < population.size(); ++i) {
        std::vector<Individual> subset;
        while (subset.size() < tournament_size) {
            int index = Random::GetUniform<int>(0, population.size() - 1);
            subset.push_back(population[index]);
        }

        // Znajdź najlepszego osobnika
        Individual best = *std::min_element(subset.begin(), subset.end(), [](const Individual& a, const Individual& b) {
            return a.rank < b.rank || (a.rank == b.rank && a.crowding_distance > b.crowding_distance);
        });
        mating_pool.push_back(best);
    }
    return mating_pool;
}

// Funkcja krzyżowania
std::vector<Individual> crossover(const std::vector<Individual>& parents) {
    std::vector<Individual> offspring;
    for (size_t i = 0; i < parents.size() - 1; i += 2) {
        Individual child(parents[i].decision_variables.size());

        double first_weight = Random::GetUniform<double>(0.1, 0.9);
        double second_weight = 1.0 - first_weight;

        for (size_t j = 0; j < child.decision_variables.size(); ++j) {
            child.decision_variables[j] = (parents[i].decision_variables[j] * first_weight +
                                           parents[i + 1].decision_variables[j] * second_weight);
            child.decision_variables[j] = std::clamp(child.decision_variables[j], 0.0L, 1.0L);
        }
        offspring.push_back(child);
    }
    return offspring;
}

// Funkcja mutacji
void mutation(std::vector<Individual>& offspring) {
    for (auto& ind : offspring) {
        for (auto& var : ind.decision_variables) {
            if (Random::GetUniform<double>(0.0, 1.0) < 0.3) { // Prawdopodobieństwo mutacji
                double perturbation = Random::GetCauchy<double>(-0.5, 0.5); // Mała zmiana
                var += perturbation;
                var = std::clamp(var, 0.0L, 1.0L); // Korekta na zakres [0, 1]
            }
        }
    }
}

void savePopulation(const std::vector<Individual>& population, int iteration, const std::string& file_name) {
    std::ofstream file(file_name, std::ios::app); // Tryb "append", aby nie nadpisywać danych
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_name << "\n";
        return;
    }
    for (const auto& ind : population) {
        file  << ind.objectives[0] << "," << ind.objectives[1] << "\n";
    }
    file.close();
}


void runExperiment(int zdt_function, int dimensions, int population_size, int max_generations) {

    // Inicjalizacja populacji
    std::vector<Individual> population(population_size, Individual(dimensions));
    initializePopulation(population, zdt_function);

    // Zmienna do przechowywania frontów
    std::vector<std::vector<Individual>> fronts;

    // Nagłówki w pliku
    //result_file << "Iteration,f1,f2\n";

    // Główna pętla algorytmu
    for (int generation = 1; generation <= max_generations; ++generation) {
        // Sortowanie niedominowane
        fronts = fastNonDominatedSort(population);

        // Obliczanie odległości tłumnej dla każdego frontu
        for (auto& front : fronts) {
            if (!front.empty()) {
                calculateCrowdingDistance(front);
            }
        }

        std::vector<Individual> pareto_front = fronts[0];  // Pierwszy front
        if (std::find(target_generations.begin(), target_generations.end(), generation) != target_generations.end()) {
            std::string filepath_pareto = "front_ZDT" + std::to_string(zdt_function) + "_" +
                                          std::to_string(dimensions) + "D_" + std::to_string(generation) + "gen.txt";

            // Zapisz pierwszy front (Pareto)
            saveParetoFront(fronts[0], filepath_pareto);
        }



        // Zbieranie wyników w określonych iteracjach
        if (std::find(target_generations.begin(), target_generations.end(), generation) != target_generations.end()) {
            std::string filepath_data= "population_ZDT" + std::to_string(zdt_function) + "_" + std::to_string(dimensions) + "D_"+std::to_string(generation)+"gen.txt";
            savePopulation(population, generation, filepath_data);
        }



        // Selekcja elitarna
        std::vector<Individual> next_population;
        for (const auto& front : fronts) {
            if (next_population.size() + front.size() <= population_size) {
                next_population.insert(next_population.end(), front.begin(), front.end());
            } else {
                std::vector<Individual> sorted_front = front;
                std::sort(sorted_front.begin(), sorted_front.end(), [](const Individual& a, const Individual& b) {
                    return a.crowding_distance > b.crowding_distance;
                });
                next_population.insert(next_population.end(), sorted_front.begin(),
                                       sorted_front.begin() + (population_size - next_population.size()));
                break;
            }
        }

        // Krzyżowanie i mutacja
        auto offspring = crossover(selection(next_population));
        mutation(offspring);

        // Ewaluacja wartości funkcji celu
        evaluateObjectives(offspring, zdt_function);

        // Łączenie populacji
        population = next_population;
        population.insert(population.end(), offspring.begin(), offspring.end());

        // Ograniczenie rozmiaru populacji
        std::sort(population.begin(), population.end(), [](const Individual& a, const Individual& b) {
            if (a.rank != b.rank) return a.rank < b.rank;
            return a.crowding_distance > b.crowding_distance;
        });
        if (population.size() > population_size) {
            population.resize(population_size);
        }
    }

}


//do uruchomienia programu python do generowania wykresu
void runPythonScript(const std::string& scriptName)
{
    std::cout << "Running Python script..." << std::endl;

    const std::string projectDirectory = PROJECT_SOURCE_DIR;
    const std::string pythonPath = projectDirectory + "/.venv/bin/python3";
    const std::string scriptPath = projectDirectory + "/" + scriptName;

#ifdef _WIN32
    const std::string command =
        "cd /d \"" + projectDirectory + "\" && python \"" + scriptPath + "\"";
#else
    const std::string command =
        "cd \"" + projectDirectory + "\" && \"" +
        pythonPath + "\" \"" + scriptPath + "\"";
#endif

    const int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Error while running the Python script." << std::endl;
    }
}

int main() {

    const int population_size = 150;
    const int max_generations = 500;

    // Eksperymenty
    const std::vector<int> dimensions = {10, 30, 50};
    const std::vector<int> zdt_functions = {1, 2, 3, 4, 6};

    for (int zdt : zdt_functions) {
        for (int dim : dimensions) {
            runExperiment(zdt, dim, population_size, max_generations);
            std::cout << "Experiment completed: ZDT" << zdt << ", " << dim << " dimensions.\n";
        }
    }

    runPythonScript("main.py");

    return 0;
}

