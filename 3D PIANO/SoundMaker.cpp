#include "SoundMaker.h"
#include <thread>
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

#include <windows.h>


using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;



std::mutex soundMutex;
ISound* currentSound = nullptr;
std::chrono::time_point<std::chrono::steady_clock> lastSoundTime;
std::string lastFilePath = "";

auto currentTime = std::chrono::steady_clock::now();

std::unordered_map<std::string, std::chrono::time_point<std::chrono::steady_clock>> lastSoundTimes;
std::vector<std::pair<std::string, int>> recordedNotes;


int currentKeyDuration = 0;
bool soundPlayed = false;
ISoundEngine* engine = createIrrKlangDevice();
float currentVolume = 1.0f;

namespace SoundMaker {

	void resetSoundFlag() {
		soundPlayed = false;
	}

	int getVolumePercentage()
	{
		int volume = currentVolume * 10;
		volume *= 10;
		return volume;
	}

	void VolumeUp() {

		currentVolume += 0.1f;
		if (currentVolume > 1.0f) currentVolume = 1.0f;
		engine->setSoundVolume(currentVolume);
		std::cout << "Volume Up: " << currentVolume * 100 << "%" << std::endl;
	}

	void VolumeDown() {

		currentVolume -= 0.1f;
		if (currentVolume < 0.0f) currentVolume = 0.0f;
		engine->setSoundVolume(currentVolume);
		std::cout << "Volume Down: " << currentVolume * 100 << "%" << std::endl;
	}

	bool wasKeyReleased(int key) {
		static bool keyPreviouslyPressed = false;
		bool keyCurrentlyPressed = GetAsyncKeyState(key) & 0x8000;

		if (!keyCurrentlyPressed && keyPreviouslyPressed) {
			keyPreviouslyPressed = false;
			return true;
		}

		if (keyCurrentlyPressed) {
			keyPreviouslyPressed = true;
		}
		return false;
	}

	bool isKeyPressedLongerThan(int key, int durationMs) {
		static bool isKeyPressed = false;
		static std::chrono::steady_clock::time_point pressStartTime;


		if (GetAsyncKeyState(key) & 0x8000) {
			if (!isKeyPressed) {

				isKeyPressed = true;
				pressStartTime = std::chrono::steady_clock::now();
			}
			else {

				auto pressDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now() - pressStartTime);
				currentKeyDuration = pressDuration.count();

				if (pressDuration.count() > durationMs) {
					return true;
				}
			}
		}
		else {

			isKeyPressed = false;
		}
		return false;
	}




	bool wasKeyPressed(int key) {
		return GetAsyncKeyState(key) & 0x8000;
	}


	void playSound(std::string filePath, bool isPressed)
	{
		std::lock_guard<std::mutex> lock(soundMutex);


		auto now = std::chrono::steady_clock::now();
		auto beginning = std::chrono::steady_clock::now();
		auto it = lastSoundTimes.find(filePath);
		int duration = 0;



		if (!engine)
			return;


		auto soundStartTime = std::chrono::steady_clock::now();

		ISound* sound = engine->play2D(filePath.c_str(), false, false, true);

		lastSoundTimes[filePath] = now;
		std::cout << "Playing sound: " << filePath << std::endl;
		isPressed = false;








	}

	/*void record(std::string filePath)
	{

	}*/
	void writeToFile()
	{

		std::cout << "DUZINA " << recordedNotes.size() << std::endl;
		std::ofstream outFile("results/file_durations.txt");
		if (!outFile) {
			std::cerr << "Error opening file for writing!" << std::endl;
			return;
		}

		for (const auto& entry : recordedNotes) {
			outFile << entry.first << ", " << entry.second << "\n";
			//std::cout << entry.first << ", " << entry.second << std::endl;
		}

		outFile.close();
		std::cout << "Data has been written to file_durations.txt" << std::endl;

		recordedNotes.clear();

	}

	void playSoundsFromFile(const std::string& fileName)
	{
		std::ifstream inFile(fileName);
		if (!inFile)
		{
			std::cerr << "Error opening file " << fileName << " for reading!" << std::endl;
			return;
		}

		std::string line;

		while (std::getline(inFile, line)) {
			std::stringstream ss(line);
			std::string filePath;
			int duration;

			// Get the file path (allow spaces in file paths)
			std::getline(ss, filePath, ','); // Reads everything before the comma as filePath

			// Get the duration (after the comma)
			ss >> duration;

			if (filePath.empty() || duration <= 0) {
				std::cerr << "Invalid entry in file: " << line << std::endl;
				continue;
			}

			std::cout << "Playing: " << filePath << " for " << duration << "ms" << std::endl;

			ISound* sound = engine->play2D(filePath.c_str(), false, false, true);


			if (sound) {

				sound->setVolume(1.0f);
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));

				// Stop the sound after the duration
				sound->stop();

			}
			else {
				std::cerr << "Error playing sound: " << filePath << std::endl;
			}
		}

		inFile.close();
		std::cout << "Finished playing all sounds from file." << std::endl;

	}

	void stop(ISoundEngine* engine)
	{
		engine->drop();
	}
}