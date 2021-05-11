/*********************************************************************************************************************
 * File : palette_kmeans.h                                                                                           *
 *                                                                                                                   *
 * 2020 Thomas Rouch                                                                                                 *
 *********************************************************************************************************************/

#ifndef PALETTE_KMEANS_H
#define PALETTE_KMEANS_H

#include <numeric>
#include <opencv2/imgproc/imgproc.hpp>

class PaletteKmeans
{
public:
    PaletteKmeans(int palette_width, int palette_height) : palette_width_(palette_width),
                                                           palette_height_(palette_height){};

    void add_image(const cv::Mat &img)
    {
        img.convertTo(data_tmp_, CV_32F);                    // Convert to float
        data_tmp_ = data_tmp_.reshape(1, data_tmp_.total()); // Flattened one channel (one triplet per row)
        cv::hconcat(full_data_, data_tmp_, full_data_);
    }

    void get_color_palette(cv::Mat &output_img, int k_clusters)
    {
        // Run kmeans
        cv::kmeans(full_data_, k_clusters, labels_, cv::TermCriteria(cv::TermCriteria::MAX_ITER, 10, 1.0), 3,
                   cv::KMEANS_PP_CENTERS, centers_);

        // Reshape to a single row of Vec3f pixels:
        centers_ = centers_.reshape(3, centers_.rows);

        // Compute cluster histogram
        std::vector<int> counts;
        counts.resize(k_clusters, 0);
        for (size_t i = 0; i < labels_.rows; i++)
            counts[labels_.at<int>(i)]++;

        // Sort colors
        std::vector<int> indices;
        indices.resize(k_clusters);
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&counts](int l, int r) {
            return counts[i] < counts[j];
        });

        // Draw palette with sorted colors
        output_img.create(palette_width_, palette_height_, CV_8U_C3);
        int x_left = 0;
        int sum_count = 0;
        for (int i = 0; i < k_clusters; i++)
        {
            /////
            //
            //
            //
            ///
            sum_count += counts[indices[i]];
            const int x_width = (sum_count * palette_width_) / k_clusters;
            cv::Rect roi(x_left, 0, x_width, palette_height);
            ref_frame(roi).setTo(color);
            x_left += x_width;

            // todo
        }
    }

private:
    int palette_width_;
    int palette_height_;

    cv::Mat full_data_;
    cv::Mat data_tmp_;

    cv::Mat labels_, centers_;
};

#endif // PALETTE_KMEANS_H