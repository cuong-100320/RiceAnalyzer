#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class RiceAnalyzer {
private:
    Mat originalImage; // Ảnh gốc
    vector<vector<Point>> contours; // Contours của các đối tượng trong ảnh

public:
    RiceAnalyzer(const string& filePath) {
        // Đọc ảnh từ đường dẫn
        originalImage = imread(filePath);

        // Kiểm tra xem việc đọc ảnh có thành công hay không
        if (originalImage.empty()) {
            cerr << "Không thể đọc ảnh." << endl;
            exit(-1);
        }
    }

    void preprocess() {
        // Chuyển đổi ảnh gốc sang không gian màu HSV
        Mat hsvImage;
        cvtColor(originalImage, hsvImage, COLOR_BGR2HSV);

        // Xác định khoảng màu để tách các đối tượng từ nền
        Scalar lowerBound(25, 50, 50);
        Scalar upperBound(100, 255, 255);

        // Tạo mặt nạ từ khoảng màu đã xác định
        Mat mask;
        inRange(hsvImage, lowerBound, upperBound, mask);

        // Tìm contours trong mặt nạ
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    }

    string analyze() {
        string message;
        // Kiểm tra số lượng contours để xác định trạng thái của lúa
        if (contours.size() < 10) {
            message = "lua binh thuong";
        }
        else {
            message = "lua co the bi sau benh";
        }
        return message;
    }

    void drawResult() {
        string message = analyze();

        // Nếu có nhiều contours, tính toán hình chữ nhật bao quanh tất cả các contours và vẽ nó lên ảnh gốc
        if (contours.size() >= 10) {
            // Tìm hình chữ nhật bao quanh tất cả các contours
            Rect boundingRect = boundingRectOfContours(contours);
            // Vẽ hình chữ nhật bao quanh tất cả các vùng bị sâu bệnh trên ảnh gốc
            rectangle(originalImage, boundingRect, Scalar(0, 0, 255), 2); // Màu đỏ, độ dày viền là 2

            // In tọa độ của hình chữ nhật lên ảnh gốc
            putText(originalImage, "Top left: (" + to_string(boundingRect.x) + ", " + to_string(boundingRect.y) + ")",
                Point(boundingRect.x, boundingRect.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
            putText(originalImage, "Bottom right: (" + to_string(boundingRect.x + boundingRect.width) + ", " + to_string(boundingRect.y + boundingRect.height) + ")",
                Point(boundingRect.x + boundingRect.width, boundingRect.y + boundingRect.height + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
        }

        // Hiển thị kết quả trên ảnh gốc
        putText(originalImage, message, Point(20, 40), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);

        // Hiển thị ảnh gốc
        imshow("Ảnh gốc", originalImage);
        waitKey(0);
    }

    // Hàm này trả về hình chữ nhật bao quanh tất cả các contours
    Rect boundingRectOfContours(const vector<vector<Point>>& contours) {
        Rect resultRect = Rect(0, 0, 0, 0); // Khởi tạo resultRect với giá trị mặc định
        if (!contours.empty()) {
            // Tìm hình chữ nhật bao quanh tất cả các điểm của contours
            int minX = INT_MAX, minY = INT_MAX;
            int maxX = INT_MIN, maxY = INT_MIN;
            for (const auto& contour : contours) {
                for (const auto& point : contour) {
                    minX = min(minX, point.x);
                    minY = min(minY, point.y);
                    maxX = max(maxX, point.x);
                    maxY = max(maxY, point.y);
                }
            }
            resultRect = Rect(minX, minY, maxX - minX, maxY - minY);
        }
        return resultRect;
    }
};

int main() {
    string filePath = "E:/anh lua/f.jpg"; // Đường dẫn đến file ảnh
    RiceAnalyzer analyzer(filePath); // Khởi tạo đối tượng RiceAnalyzer với ảnh từ đường dẫn
    analyzer.preprocess(); // Tiền xử lý ảnh
    analyzer.drawResult(); // Hiển thị kết quả

    return 0;
}
