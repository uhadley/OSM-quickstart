#include <fstream>
#include <string>

class svg {
private:
    std::ofstream svgFile;

    // Private helper to write line to the SVG file
    void drawLineInternal(int x1, int y1, int x2, int y2, const std::string& color) {
        
        svgFile << "<line x1=\"" << x1 << "\" y1=\"" << y1
                << "\" x2=\"" << x2 << "\" y2=\"" << y2
                << "\" stroke=\"" << color << "\" stroke-width=\"2\" />\n";
    }

public:
    // Constructor: Open SVG file, write header
    svg(const std::string& filename, int width = 500, int height = 500) {
        svgFile.open(filename);
        if (!svgFile.is_open()) {
            throw std::runtime_error("Failed to open SVG file.");
        }

        // Write SVG header
        svgFile << "<?xml version=\"1.0\" standalone=\"no\"?>\n";
        svgFile << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
                   "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
        svgFile << "<svg width=\"" << width << "\" height=\"" << height 
                << "\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    }

    // Destructor: Close SVG file and write footer
    ~svg() {
        if (svgFile.is_open()) {
            svgFile << "</svg>\n";
            svgFile.close();
        }
    }

    // Public method to draw a line
    void draw_line(int x1, int y1, int x2, int y2, const color& clr) {
                drawLineInternal(x1, y1, x2, y2, clr.tostr());
    }
};
