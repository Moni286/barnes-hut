#pragma once

#include <cmath>


namespace geometry {
	struct Vector2d {
		double x;
		double y;

		Vector2d() { x = 0; y = 0; }
		Vector2d(double x, double y) { this->x = x; this->y = y; }

		Vector2d(const Vector2d &val) {
			x = val.x;
			y = val.y;
		}

		double magnitude() {
			return std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2));
		}

		Vector2d direction() {
			return (*this) / magnitude();
		}

		Vector2d & operator=(const Vector2d & rhs) {
			x = rhs.x;
			y = rhs.y;
			return *this;
		}

		Vector2d operator-(const Vector2d subtrahend) const {
			return Vector2d(x - subtrahend.x, y - subtrahend.y);
		}

		Vector2d operator+(const Vector2d& addend) const {
			return Vector2d(x + addend.x, y + addend.y);
		}

		Vector2d operator^(const double exponent) const {
			return Vector2d(std::pow(x, exponent), std::pow(y, exponent));
		}

		friend Vector2d operator*(const double multiplicand, const Vector2d &multiplier) {
			return Vector2d(multiplicand * multiplier.x, multiplicand * multiplier.y);
		}

		friend Vector2d operator/(const double dividend, const Vector2d &divisor) {
			return Vector2d(dividend / divisor.x, dividend / divisor.y);
		}

		friend Vector2d operator/(const Vector2d dividend, const double &divisor) {
			return (1 / divisor) * dividend;
		}

		friend bool operator==(const Vector2d & lhs, const Vector2d & rhs) {
			return lhs.x == rhs.x && lhs.y == rhs.y;
		}
		
	};

	struct Rectangle {
		Vector2d topLeft;
		Vector2d bottomRight;

		Rectangle() { topLeft = Vector2d(); bottomRight = Vector2d(); }
		Rectangle(const Rectangle &rect) { this->bottomRight = rect.bottomRight; this->topLeft = rect.topLeft; }
		Rectangle(const Vector2d & topLeft, const Vector2d & bottomRight) { this->topLeft = topLeft; this->bottomRight = bottomRight; }
	};
}
