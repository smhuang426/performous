#include "songwriter.hh"
#include <fstream>
#include <iostream>

void writeMid(Song const& s, boost::filesystem::path const& filename) {
	std::ofstream f(filename.string().c_str(), std::ios::binary);
	// FIXME: The following is just an example and doesn't actually output MID format
	char buf[1024] = {};
	Notes const& notes = s.vocals.notes;
	std::cout << notes.size() << std::endl;
	for (unsigned int i = 0; i < notes.size(); ++i) {
		Note const& n = notes[i];
		buf[0] = 0xFF;
		buf[1] = n.note; // MIDI note value
		// Others are n.begin, n.end, n.type etc. (see notes.hh)
		f.write(buf, 1024);
	}
}

void writeIni(Song const& s, boost::filesystem::path const& filename) {
	std::ofstream f(filename.string().c_str(), std::ios::binary);
	f << "[song]\n";
	f << "title=" << s.title << '\n';
	// ...
}

void writeToFile(Song const& s, boost::filesystem::path const& path) {
	create_directory(path);
	writeIni(s, path / "song.ini");
	writeMid(s, path / "notes.mid");
}

