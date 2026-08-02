import matplotlib
#matplotlib.use('TkAgg')  #dla macos
import matplotlib.pyplot as plt
import pandas as pd
import os
from glob import glob

# generacje i kolory
target_generations = [20, 50, 100, 500]
generation_colors = {
    20: '#FF0000',  # czerwony
    50: '#FFA500',  # pomarańczowy
    100: '#008000',  # zielony
    500: '#0000FF'  # niebieski
}


# wczytanie danych z pliku .txt
def load_data(filepath):
    data = pd.read_table(filepath, header=None, names=["f1", "f2"], sep=",")
    return data


# wyodrebnienie z nazw plików danych o eksperymencie tj. numer funkcji, liczba wymiarow i numer generacji
def parse_file_name(filename):
    parts = filename.split("_")
    function = parts[1].replace("ZDT", "")
    dimensions = parts[2].replace("D", "").replace("gen.txt", "")
    generation = int(parts[3].replace("gen.txt", "").replace("gen", ""))
    return function, dimensions, generation

# rysowanie wspolnego wykresu dla funkcji testowej i wymiaru, łącząc dane z kolejnych generacji
def plot_experiment_for_function_and_dimension(function, dimension, population_files, pareto_files):
    plt.figure(figsize=(12, 10))
    legend_labels_added = set()

    for generation in target_generations:
        # znalezienie plikó dla populacji i dla frontu
        pop_file = next((f for f in population_files if f"_{generation}gen.txt" in f), None)
        pareto_file = next((f for f in pareto_files if f"_{generation}gen.txt" in f), None)

        if pop_file and pareto_file:
            # wczytanie danych populacji i frontu
            population_data = load_data(pop_file)
            pareto_front_data = load_data(pareto_file)

            # rysowanie dane populacji jako punkty
            plt.scatter(population_data['f1'], population_data['f2'], color=generation_colors[generation], s=10,
                        label=f"Iteration {generation}", alpha=0.5)

            # rysowanie frontu jako linii
            pareto_front_sorted = pareto_front_data.sort_values(by="f1")
            plt.plot(pareto_front_sorted['f1'], pareto_front_sorted['f2'], color=generation_colors[generation],
                     marker='o', linewidth=1.5)

            if generation not in legend_labels_added:
                plt.scatter([], [], color=generation_colors[generation])
                legend_labels_added.add(generation)

    # tytuł wykresu
    title = f"NSGA-II ZDT{function} ({dimension}D)\nIterations: 500, Population size: 150, Dimensions: {dimension}"
    plt.title(title, fontsize=10)

    # etykiety osi
    plt.xlabel("f1")
    plt.ylabel("f2")

    # legenda pod wykresem
    plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.1), ncol=len(target_generations), fontsize=9)

    #  linie pomocnicze
    plt.grid(which='major', color='black', linestyle='-', linewidth=0.7)
    plt.grid(which='minor', color='gray', linestyle='--', linewidth=0.5)
    plt.minorticks_on()

    # eksport wykresów do .png
    output_filename = f"ZDT{function}_{dimension}D_plot.png"
    plt.savefig(output_filename)
    print(f"Plot saved as: {output_filename}")
    plt.show()


population_files_all = sorted(glob("population_ZDT*_*.txt"))  # dane populacji
pareto_files_all = sorted(glob("front_ZDT*_*.txt"))  # dane frontów

# grupoanie plików wg funkcji testowej i wymiarów
experiments = {}
for population_file, pareto_file in zip(population_files_all, pareto_files_all):
    function, dimensions, generation = parse_file_name(population_file)

    key = (function, dimensions)
    if key not in experiments:
        experiments[key] = {"population_files": [], "pareto_files": []}

    experiments[key]["population_files"].append(population_file)
    experiments[key]["pareto_files"].append(pareto_file)

# tworzenie wykresów dla każdej funkcji testowej i wymiaru
for (function, dimension), files in experiments.items():
    population_files = files["population_files"]
    pareto_files = files["pareto_files"]
    plot_experiment_for_function_and_dimension(function, dimension, population_files, pareto_files)