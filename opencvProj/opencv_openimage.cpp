/*
*
* Robotic Vision final project
* Using the Tesseract OCR API with the Extremal Region Filter algorithm, we can detect text in images
* For our purposes, the feed comes from the drone's camera, which is then process to extract text out of it
* Created on: November 30, 2017
*/

#include "ardrone\ardrone.h"
#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <queue>
#include <set>
#include <time.h>

using namespace std;
using namespace cv;
using namespace cv::text;


// AR.Drone class
ARDrone ardrone;

void forward(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(0.5, 0.0, 0.0, 0.0);
  }
}

void backward(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(-0.5, 0.0, 0.0, 0.0);
  }
}

void hover(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(0.0, 0.0, 0.0, 0.0);
  }
}

void ascend(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(0.0, 0.0, 0.5, 0.0);
  }
}

void fall(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(0.0, 0.0, -0.5, 0.0);
  }
}

void back(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(-0.5, 0.0, 0.0, 0.0);
  }
}

void left(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(0.0, 0.5, 0.0, 0.0);
  }
}

void right(int seconds) {
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC;
  while (clock() < endwait) {
    ardrone.move3D(0.0, -0.5, 0.0, 0.0);
  }
}

bool isRepetitive(const string& s);
// Draw extremal regions in an image via floodfill
void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat &segmentation);

void drone_instruction(string cmd) { 
  if (cmd == "ascend") {
    ascend(1);
  }
  else if (cmd == "rise") {
    ascend(1);
  }
  else if (cmd == "left") {
    left(1);
  }
  else if (cmd == "right") {
    right(1);
  }
  else if (cmd == "advance") {
    forward(1);
  }
  else if (cmd == "sleep") {
    hover(1);
  }
  else if (cmd == "fall") {
    fall(1);
  }
  else if (cmd == "die") {
    ardrone.landing();
    ardrone.close();
    exit(0);
  }
  else if (cmd == "descend") {
    fall(1);
  }
  else if (cmd == "forward") {
    forward(1);
  }
  else if (cmd == "backward") {
    backward(1);
  }
  else if (cmd == "down") {
    fall(1);
  }
  else assert(0);
}

int main(int argc, char* argv[]) {
  cout << "Robotic vision AGO-DIC 2017 Final Project" << endl;
  cout << "Ivan Soto, Gerardo Garcia" << endl;

  set<string> commands;
  commands.insert("ascend");
  commands.insert("rise");
  commands.insert("left");
  commands.insert("right");
  commands.insert("advance");
  commands.insert("sleep");
  commands.insert("fall");
  commands.insert("die");
  commands.insert("descend");
  commands.insert("forward");
  commands.insert("backward");
  commands.insert("down");

  // Initialize
  if (!ardrone.open()) {
    std::cout << "Failed to initialize." << std::endl;
    return -1;
  }

  // Battery
  std::cout << "Battery = " << ardrone.getBatteryPercentage() << "[%]" << std::endl;

  // lists of commands to do
  queue<string> todo;
  ardrone.takeoff();

  // constantly receiving feed
  while (true) {
    int key = waitKey(1);
    if (key == 'l') {
      ardrone.landing();
      ardrone.close();
      exit(0);
    }
    while (!todo.empty()) {
      string front = todo.front();
      todo.pop();
      cout << "executing: " << front << endl;
      drone_instruction(front);
      hover(1);
    }
    Mat image = ardrone.getImage();
    imshow("camera", image);

    // ERFilters boilerplate

    // Extract channels to be processed individually
    vector<Mat> channels;

    Mat grey;
    cvtColor(image, grey, COLOR_RGB2GRAY);
    channels.push_back(grey);
    channels.push_back(255 - grey);

    double t_d = (double) getTickCount();
    // Create ERFilter objects with the 1st and 2nd stage default classifiers
    Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("trained_classifierNM1.xml"), 8, 0.00015f, 0.13f, 0.2f, true, 0.1f);
    Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("trained_classifierNM2.xml"), 0.5);

    vector< vector<ERStat> > regions(channels.size());
    // Apply the default cascade classifier to each independent channel (could be done in parallel)
    for (int c = 0; c < (int) channels.size(); c++) {
      er_filter1->run(channels[c], regions[c]);
      er_filter2->run(channels[c], regions[c]);
    }
    
    double t_g = (double) getTickCount();
    
    // Detect character groups
    vector< vector<Vec2i> > nm_region_groups;
    vector<Rect> nm_boxes;
    erGrouping(image, channels, regions, nm_region_groups, nm_boxes, ERGROUPING_ORIENTATION_HORIZ);
    
    // No boxes detected, get new feed
    if (nm_boxes.empty()) continue;

    // Text Recognition (OCR)

    double t_r = (double)getTickCount();
    Ptr<OCRTesseract> ocr = OCRTesseract::create();
    string output;
    vector<string> words_detection;

    t_r = (double) getTickCount();
    cout << nm_boxes.size() << endl;
    for (int i = 0; i < (int) nm_boxes.size(); i++) {
      Mat group_img = Mat::zeros(image.rows + 2, image.cols + 2, CV_8UC1);
      er_draw(channels, regions, nm_region_groups[i], group_img);
      Mat group_segmentation;
      group_img.copyTo(group_segmentation);
    
      group_img(nm_boxes[i]).copyTo(group_img);
      copyMakeBorder(group_img, group_img, 15, 15, 15, 15, BORDER_CONSTANT, Scalar(0));

      vector<Rect> boxes;
      vector<string> words;
      vector<float> confidences;
      ocr->run(group_img, output, &boxes, &words, &confidences, OCR_LEVEL_WORD);
      vector<int> p;
      output.erase(remove(output.begin(), output.end(), '\n'), output.end());
      if (output.size() < 3)
        continue;

      for (int j = 0; j < (int) boxes.size(); j++) {
        boxes[j].x += nm_boxes[i].x - 15;
        boxes[j].y += nm_boxes[i].y - 15;

        // ignore using probability
        if ((words[j].size() < 2) || (confidences[j] < 51) || ((words[j].size() == 2) && (words[j][0] == words[j][1])) || ((words[j].size()< 4) && (confidences[j] < 60)) || isRepetitive(words[j]))
          continue;
        words_detection.push_back(words[j]);
      }
    
    }
    vector<int> p;
    for (int j = 0; j < (int) words_detection.size(); j++) p.push_back(j);
    // sort permutation by boxes' y coordinate
    sort(p.begin(), p.end(), [&](const int &x, const int &y) {
      if (nm_boxes[x].y == nm_boxes[y].y) return nm_boxes[x].x < nm_boxes[y].x;
      return nm_boxes[x].y < nm_boxes[y].y;
    });
    // lowercase all words and add to todo queue 
    for (int i = 0; i < (int) words_detection.size(); i++) {
      string cmd = words_detection[p[i]];
      for (int i = 0; i < (int)cmd.size(); i++) {
        if ('A' <= cmd[i] && cmd[i] <= 'Z') cmd[i] = cmd[i] - 'A' + 'a';
      }
      if (!commands.count(cmd)) continue;
      todo.push(cmd);
    }
  }
  ardrone.close();
  return 0;
}

bool isRepetitive(const string &s) {
  int cnt = 0;
  for (int i = 0; i < (int) s.size(); i++) {
    if (s[i] == 'i' || s[i] == 'l' || s[i] == 'I') {
      cnt++;
    }
  }
  if (count > ((int) s.size() + 1) / 2) return true;
  return false;
}


void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat& segmentation) {
  for (int r = 0; r < (int) group.size(); r++) {
    ERStat er = regions[group[r][0]][group[r][1]];
    if (er.parent != NULL) {
      int newMaskVal = 255;
      int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
      floodFill(channels[group[r][0]], segmentation, Point(er.pixel % channels[group[r][0]].cols, er.pixel / channels[group[r][0]].cols), Scalar(255), 0, Scalar(er.level), Scalar(0), flags);
    }
  }
}

