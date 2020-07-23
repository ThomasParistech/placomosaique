/*********************************************************************************************************************
 * File : array_to_html.h                                                                                            *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef ARRAY_TO_HTML_H
#define ARRAY_TO_HTML_H

#include <fstream>
#include <functional>

struct ArrayToHtmlCaptions
{
    std::string page_large_title;
    std::string page_title;
    std::string table_caption;
};

/// @brief Converts 2D vector to HTML
/// @param output_filename Name of the HTML file to generate (without the .html extension)
/// @param n_rows Number of rows of the 2D array
/// @param n_cols Number of columns of the 2D array
/// @param array_getter Getter to a 2D element of the flattened version of the array that we want to save in Html
/// @param captions Text surrounding the table in the Html page
template <typename T>
void array_to_html(const std::string &output_filename,
                   int n_rows, int n_cols, const std::function<T(int)> array_getter,
                   const ArrayToHtmlCaptions &captions = ArrayToHtmlCaptions())
{
    std::string file_path = "/tmp/" + output_filename + ".html";
    std::ofstream output_file;
    output_file.open(file_path);
    output_file << "<!DOCTYPE html>\n"
                << "<html>\n"
                << "<head>\n";

    // Style
    output_file << "<style>\n"
                << "table {\n"
                << "  font-family: arial, sans-serif;\n"
                << "  border-collapse: collapse;\n"
                << "  width: 100%;\n"
                << "}\n"

                << "td, th {\n"
                << "  border: 1px solid #dddddd;\n"
                << "  text-align: left;\n"
                << "  padding: 8px;\n"
                << "}\n"
                << "tr:nth-child(even) {\n"
                << "  background-color: #dddddd;\n"
                << "}\n"
                << "</style>\n";

    output_file << "</head>\n"
                << "<body>\n";

    // Captions
    if (!captions.page_large_title.empty())
        output_file << "<h2>" << captions.page_large_title << "</h2>\n";
    if (!captions.page_title.empty())
        output_file << "<p>" << captions.page_title << "</p>\n";

    // Table
    output_file << "<table style=\" width : 100 % \">\n";
    if (!captions.table_caption.empty())
        output_file << "<caption>" << captions.table_caption << "</caption>\n";

    // Header
    output_file << "<tr>\n"
                << "<th>&nbsp;</th>\n";
    for (size_t j = 0; j < n_cols; j++)
        output_file << "<th>" << j << "</th>\n";
    output_file << "</tr>\n";

    // Rows of the 2D vector
    for (size_t i = 0; i < n_rows; i++)
    {
        output_file << "<tr>\n"
                    << "<th>" << i << "</th>\n";
        const int i0 = i * n_rows;
        for (size_t j = 0; j < n_cols; j++)
            output_file << "<td>" << array_getter(i0 + j) << "</td>\n";
        output_file << "</tr>\n";
    }
    output_file << "</table>\n"
                << "</body>\n"
                << "</html>\n";
    output_file.close();
}

#endif // ARRAY_TO_HTML_H
