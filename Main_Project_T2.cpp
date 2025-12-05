#include "Project.h"
#include <iostream>
#include <fstream> 
#include <sstream> 
#include <string>
#include <algorithm>
#include <cctype>
#include <exception> // for error handling
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

const std::string BASE_URL = "https://api.themoviedb.org/3/search/movie";

std::string getApiKey() {
    std::ifstream keyFile("api_key.txt");
    std::string key;

    if (keyFile.is_open() && std::getline(keyFile, key)) {
        key.erase(std::remove(key.begin(), key.end(), '\n'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\r'), key.end());
        return key;
    }

    std::cerr << "FATAL ERROR : Could not read API Key, file does not exist." << std::endl;
    exit(1);
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// for case-insensitive search
std::string toLower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c){ return std::tolower(c); });
    return lower_str;
}

/**
 * @brief Fetches movie data from the TMDb API, processes JSON, and adds results to Deque.
 * NOTE: This REPLACES the old loadMoviesFromFile function.
 */
void fetchAndProcessMovies(Deque<Movie>& dq, const std::string& searchTerm, const std::string& apiKey) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // 1. URL ENCODING and CONSTRUCTION
    std::string encodedQuery = "";
    CURL *curl_encode = curl_easy_init();
    if (curl_encode) {
        // Encodes the search term for use in a URL
        char *output = curl_easy_escape(curl_encode, searchTerm.c_str(), searchTerm.length());
        if (output) {
            encodedQuery = output;
            curl_free(output);
        }
        curl_easy_cleanup(curl_encode);
    }
    
    // CONSTRUCT THE FULL API URL
    std::string url = BASE_URL + "?api_key=" + apiKey + "&query=" + encodedQuery;
    
    // 2. CURL SETUP AND EXECUTION
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        std::cout << "Searching API for '" << searchTerm << "'...\n";
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "cURL error: " << curl_easy_strerror(res) << std::endl;
            return;
        }

        // 3. JSON PARSING
        try {
            auto json_response = json::parse(readBuffer);
            if (json_response.contains("results") && json_response["results"].is_array()) {
                const auto& results = json_response["results"];
                
                std::cout << "\n--- Top API Results Added ---\n";
                int count = 0;
                
                for (const auto& item : results) {
                    // Check for required fields before extraction
                    if (item.contains("title") && item["title"].is_string() &&
                        item.contains("vote_average") && item["vote_average"].is_number() &&
                        item.contains("release_date") && item["release_date"].is_string()) 
                    {
                        std::string title = item["title"];
                        double rating = item["vote_average"].get<double>();
                        
                        // Extract year from YYYY-MM-DD date string
                        int year = 0;
                        std::string date_str = item["release_date"].get<std::string>();
                        
                        if (date_str.length() >= 4) {
                            try {
                                year = std::stoi(date_str.substr(0, 4));
                            } catch (...) { /* ignore parse failure */ }
                        }
                        
                        // Create and add the Movie to the Deque (acting as a cache)
                        dq.push_front(Movie(title, year, rating));
                        std::cout << "✅ Added: " << title << " (" << year << ") - Rating: " << rating << "\n";
                        count++;
                        
                        // Limit results added to 5 per search query
                        if (count >= 5) break; 
                    }
                }
                if (count == 0) {
                    std::cout << "No relevant results found for '" << searchTerm << "'.\n";
                }
            }
        } catch (const json::parse_error& e) {
            std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        }
    }
}

int main() {
    std::cout << "--- DEQUE PROJECT : MOVIE CATALOGUE ---\n";

   curl_global_init(CURL_GLOBAL_DEFAULT);

   const std::string API_KEY = getApiKey();

   Deque<Movie> movieQueue;

   std::string userInput;

   while (true) {
    std::cout << "\n--------------------------------------------------\n";
    std::cout << "Deque size (cached results): " << movieQueue.size() << "\n";
    std::cout << "Enter a movie title to SEARCH the TMDb API (or type 'quit' to exit):\n> ";
    std::getline(std::cin, userInput);

    if (toLower(userInput) == "quit") {
        std::cout << "Exiting Program. Bye!\n";
        break;
    }

    if (!userInput.empty()) {
        fetchAndProcessMovies(movieQueue, userInput, API_KEY);
    }
   }

   curl_global_cleanup();

   std::cout << "\nFinal Deque Size: " << movieQueue.size() << " cached results.\n";

   return 0;
}