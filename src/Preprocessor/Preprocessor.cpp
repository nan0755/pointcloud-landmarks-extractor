//
// Created by akhil on 17.05.17.
//
#include "Preprocessor/Preprocessor.hpp"

void Preprocessor::outlierRemover(int mean, double stdDev){
    // Create the filtering object
    pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
    sor.setInputCloud (processCloud);
    sor.setMeanK (mean);
    sor.setStddevMulThresh (stdDev);
    sor.filter (*processCloud);
}

void Preprocessor::groundPlaneRemover(double distThreshold) {
    // Create the segmentation object for the planar model and set all the parameters
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
    pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plane (new pcl::PointCloud<pcl::PointXYZ> ());
    seg.setOptimizeCoefficients (true);
    seg.setModelType (pcl::SACMODEL_PLANE);
    seg.setMethodType (pcl::SAC_RANSAC);
    seg.setMaxIterations (100);
    seg.setDistanceThreshold (distThreshold);

    int j=0, nr_points = (int) processCloud->points.size ();
    while (processCloud->points.size () > 0.3 * nr_points)
    {
        // Segment the largest planar component from the remaining cloud
        seg.setInputCloud (processCloud);
        seg.segment (*inliers, *coefficients);
        if (inliers->indices.size () == 0)
        {
            std::cout << "Could not estimate a planar model for the given dataset." << std::endl;
            break;
        }
        // Extract the planar inliers from the input cloud
        pcl::ExtractIndices<pcl::PointXYZ> extract;
        extract.setInputCloud (processCloud);
        extract.setIndices (inliers);
        extract.setNegative (true);
        extract.filter (*processCloud);
        j++;
    }
}

void Preprocessor::cutCloudBuilder(pcl::PointCloud<pcl::PointXYZ>::Ptr cutCloud, double zMin, double zMax) {
    // Create the filtering object
    pcl::PassThrough<pcl::PointXYZ> pass;
    pass.setInputCloud (processCloud);
    pass.setFilterFieldName ("z");
    pass.setFilterLimits (zMin, zMax);
    pass.filter(*cutCloud);
}

