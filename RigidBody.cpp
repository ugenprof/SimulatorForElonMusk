#include "RigidBody.h"

RigidBodyParameters::RigidBodyParameters()
	: position(Vector2f(0, 0)), width(0), height(0), angle(0), mass(0),
	moment_of_inertia(0), mass_position(Vector2f(0, 0)),
	velocity(Vector2f(0, 0)), acceleration(Vector2f(0, 0)), angle_velocity(0),
	angle_acceleration(0), 
	collision_vertex({Point(0, 0), Point(1, 0), Point(1, 1), Point(0, 1)}) {}

RigidBodyParameters::RigidBodyParameters(const Vector2f new_position,
	const float& w, const float& h, const float& start_angle,
	const float& start_mass, const float& start_moment_of_inertia,
	const Vector2f& start_mass_position)
	: position(new_position), width(w), height(h), angle(start_angle), mass(start_mass),
	moment_of_inertia(start_moment_of_inertia), mass_position(start_mass_position),
	velocity(Vector2f(0, 0)), acceleration(Vector2f(0, 0)), angle_velocity(0),
	angle_acceleration(0), 
	collision_vertex({ Point(0, 0), Point(1, 0), Point(1, 1), Point(0, 1) }) {}

RigidBodyParameters::RigidBodyParameters(const Vector2f new_position,
	const float& w, const float& h, const float& start_angle,
	const float& start_mass, const float& start_moment_of_inertia,
	const Vector2f& start_mass_position, const Vector2f& start_velocity,
	const Vector2f& start_acceleration, const float& start_angle_velocity,
	const float& start_angle_acceleration) 
	: position(new_position), width(w), height(h), angle(start_angle), mass(start_mass),
	moment_of_inertia(start_moment_of_inertia), mass_position(start_mass_position),
	velocity(start_velocity), acceleration(start_acceleration), angle_velocity(start_angle_velocity),
	angle_acceleration(start_angle_acceleration), 
	collision_vertex({ Point(0, 0), Point(1, 0), Point(1, 1), Point(0, 1) }) {}

RigidBody::RigidBody(const String& f, const RigidBodyParameters& parameters)
	: Object(f, parameters.position, parameters.width, parameters.height, parameters.angle),
	mass(parameters.mass), moment_of_inertia(parameters.moment_of_inertia), mass_position(parameters.mass_position),
	velocity(parameters.velocity), acceleration(parameters.acceleration), angle_velocity(parameters.angle_velocity),
	angle_acceleration(parameters.angle_acceleration), collision_vertex(parameters.collision_vertex) {
	diag = sqrt(pow(GetWidth() * GetMassPosition().x, 2) + pow(GetHeight() * GetMassPosition().y, 2));
	if (GetWidth() * GetMassPosition().x != 0) {
		b = atan((GetHeight() * GetMassPosition().y) / (GetWidth() * GetMassPosition().x));
	}
	else { b = 0; }
}

float RigidBody::GetMass() const { return mass; }
float RigidBody::GetMomentOfInertia() const { return moment_of_inertia; }
Vector2f RigidBody::GetMassPosition() const { return mass_position; }
Vector2f RigidBody::GetVelocity() const { return velocity; }
Vector2f RigidBody::GetAcceleration() const { return acceleration; }
float RigidBody::GetAngleVelocity() const { return angle_velocity; }
float RigidBody::GetAngleAcceleration() const { return angle_acceleration; }
String RigidBody::GetStatusText() const { return status_text; }
Force RigidBody::GetForce(const std::string& name) const { 
	if (forces.count(name) == 1) {
		return forces.at(name);
	}
	else { return Force(true, 0, { 0, 0 }, {0, 0}); }
}

void RigidBody::SetMass(const float& new_mass) { mass = new_mass; }
void RigidBody::SetMomentOfInertia(const float& new_moment_of_inertia) { moment_of_inertia = new_moment_of_inertia; }
void RigidBody::SetMassPosition(const Vector2f& new_mass_position) { mass_position = new_mass_position; }
void RigidBody::SetVelocuty(const Vector2f& new_velocity) { velocity = new_velocity; }
void RigidBody::SetAcceleration(const Vector2f& new_acceleration) { acceleration = new_acceleration; }
void RigidBody::SetAngleVelocity(const float& new_angle_velocity) { angle_velocity = new_angle_velocity; }
void RigidBody::SetAngleAcceleration(const float& new_angle_acceleration) { angle_acceleration = new_angle_acceleration; }

Vector2f RigidBody::GetAbsMassPosition() const {
	return { position.x + mass_position.x * width, position.y - mass_position.y * height };
}

void RigidBody::UpdatePosition(const float& dt) {
	Vector2f new_position;
	float new_angle;

	new_position.x = GetPosition().x + velocity.x * dt -
		RAD * angle_velocity * diag * dt * cos(RAD * GetAngle() + b + PI / 2);
	new_position.y = GetPosition().y + velocity.y * dt -
		RAD * angle_velocity * diag * dt * sin(RAD * GetAngle() + b + PI / 2);

	new_angle = GetAngle() + angle_velocity * dt;

	SetVelocuty(Vector2f(velocity.x + acceleration.x * dt, velocity.y + acceleration.y * dt));
	SetAngleVelocity(angle_velocity + angle_acceleration * dt);

	SetPosition(new_position, new_angle);
	UpdateForces();
}

void RigidBody::AddForce(const std::string& name, const Force& new_force) {
	forces[name] = new_force;
}

void RigidBody::DeleteForce(const std::string& name) {
	forces.erase(name);
}

void RigidBody::ForceOn(const std::string& name) {
	forces[name].exist = true;
}
void RigidBody::ForceOff(const std::string& name) {
	forces[name].exist = false;
}

void RigidBody::UpdateForces() {
	Vector2f new_acceleration(0, 0);
	float new_angle_accelaration = 0;

	for (auto i : forces) {
		if (i.second.exist == true) {
			if (i.second.is_force_field == true) {
				new_acceleration.x += i.second.force_vector.x;
				new_acceleration.y += i.second.force_vector.y;
			}
			else {
				float Fx = i.second.force * i.second.force_vector.x;
				float Fy = i.second.force * i.second.force_vector.y;

				new_acceleration.x += (Fx * cos(RAD * GetAngle()) - Fy * sin(RAD * GetAngle())) / mass;
				new_acceleration.y += (Fx * sin(RAD * GetAngle()) + Fy * cos(RAD * GetAngle())) / mass;

				new_angle_accelaration += (Fx * GetHeight() * (mass_position.y - i.second.force_point.y)) / moment_of_inertia;
				new_angle_accelaration -= (Fy * GetWidth() * (mass_position.x - i.second.force_point.x)) / moment_of_inertia;
			}
		}
	}

	SetAcceleration(new_acceleration);
	SetAngleAcceleration(new_angle_accelaration);
}

Vector2f RigidBody::GetCenterPosition() const {
	return Vector2f{
		GetPosition().x + diag * cos(RAD * GetAngle() + b),
		GetPosition().y + diag * sin(RAD * GetAngle() + b)
	};
}

void RigidBody::DrawMassPosition(RenderWindow& window) const {
	CircleShape Cshape(10.f);
	Cshape.setFillColor(Color::Red);
	Cshape.setPosition(GetCenterPosition() - Vector2f(10, 10));
	window.draw(Cshape);
}

void RigidBody::DrawBodyWay(RenderWindow& window) {
	way.setPrimitiveType(LinesStrip);
	way.append(Vertex(Vector2f(GetPosition().x + diag * cos(RAD * GetAngle() + b),
		GetPosition().y + diag * sin(RAD * GetAngle() + b)),
		Color::Red
	));
	window.draw(way);
}

void RigidBody::DeleteBodyWay(RenderWindow& window) {
	way.clear();
}

void RigidBody::DrawForce(RenderWindow& window, const Force& force) const {
	if (force.exist == true) {
		VertexArray force_line;
		force_line.setPrimitiveType(Lines);

		if (force.is_force_field == false) {
			float diag_force = sqrt(pow(GetWidth() * force.force_point.x, 2) + pow(GetHeight() * force.force_point.y, 2));
			float diag_force_end = sqrt(
				pow(GetWidth() * force.force_point.x + force.force * force.force_vector.x, 2) +
				pow(GetHeight() * force.force_point.y + force.force * force.force_vector.y, 2)
			);
			float x, y;

			float fb;
			x = GetWidth() * force.force_point.x;
			y = GetHeight()* force.force_point.y;

			if (x >= 0 && y != 0) { fb = atan(y / x); }
			else if (x < 0) { fb = atan(y / x) - PI; }
			else { fb = 0; }

			float feb;
			x = GetWidth() * force.force_point.x + force.force * force.force_vector.x;
			y = GetHeight()* force.force_point.y + force.force * force.force_vector.y;

			if (x >= 0 && y != 0) { feb = atan(y / x); }
			else if (x < 0) { feb = atan(y / x) - PI; }
			else { feb = 0; }

			force_line.append(Vertex(Vector2f(
				GetPosition().x + cos(RAD * GetAngle() + fb) * diag_force,
				GetPosition().y + sin(RAD * GetAngle() + fb) * diag_force),
				Color::Green
			));
			force_line.append(Vertex(Vector2f(
				GetPosition().x + cos(RAD * GetAngle() + feb) * diag_force_end,
				GetPosition().y + sin(RAD * GetAngle() + feb) * diag_force_end),
				Color::Green
			));
		}
		else {
			force_line.append(Vertex(Vector2f(
				GetPosition().x + cos(RAD * GetAngle() + b) * diag,
				GetPosition().y + sin(RAD * GetAngle() + b) * diag),
				Color::Green
			));
			force_line.append(Vertex(Vector2f(
				GetPosition().x + cos(RAD * GetAngle() + b) * diag + force.force_vector.x * mass,
				GetPosition().y + sin(RAD * GetAngle() + b) * diag + force.force_vector.y * mass),
				Color::Green
			));
		}
		window.draw(force_line);
	}
}


void RigidBody::DrawSpeed(RenderWindow& window) const {
	VertexArray speed_line;
	speed_line.setPrimitiveType(Lines);

	speed_line.append(Vertex(Vector2f(
		GetPosition().x + cos(RAD * GetAngle() + b) * diag,
		GetPosition().y + sin(RAD * GetAngle() + b) * diag),
		Color::Blue
	));
	speed_line.append(Vertex(Vector2f(
		GetPosition().x + cos(RAD * GetAngle() + b) * diag + velocity.x * mass,
		GetPosition().y + sin(RAD * GetAngle() + b) * diag + velocity.y * mass),
		Color::Blue
	));
	window.draw(speed_line);
}


int RigidBody::GetFlyStatus() const { return fly_status; }
void RigidBody::SetFlyStatus(const int& new_status) { fly_status = new_status; }

bool RigidBody::LandingCheck(const Surface& s) {
	long mid_iter = s.Get_iter_0() + 2 * GetCenterPosition().x / s.Get_spacing();
	long start = mid_iter - sqrt(pow(height, 2) + pow(width, 2)) / (2 * s.Get_spacing());
	long end = mid_iter + sqrt(pow(height, 2) + pow(width, 2)) / (2 * s.Get_spacing());
	start = (start / 2) * 2;
	end = (end / 2) * 2;
	Line surface_line(Point(s.GetVertex(start).position.x, s.GetVertex(start).position.y),
		Point(s.GetVertex(end).position.x, s.GetVertex(end).position.y));

	int ship_angle = GetAngle();
	ship_angle %= 360;
	int surface_angle = surface_line.GetAngle();

	timer.restart();

	if (sqal(GetVelocity()) > MAX_VELOCITY) { 
		if (GetFlyStatus() == 0) { SetFlyStatus(2); }
		return false; 
	}
	if (GetAngleVelocity() > MAX_ANGLE_VELOCITY) { 
		if (GetFlyStatus() == 0) { SetFlyStatus(3); }
		return false; 
	}
	if (mod(surface_angle) > MAX_ANGLE) { 
		if (GetFlyStatus() == 0) { SetFlyStatus(4); }
		return false; 
	}
	if (mod(surface_angle - ship_angle) > MAX_ANGLE_BETWEEN) { 
		if (GetFlyStatus() == 0) { SetFlyStatus(5); }
		return false; 
	}
	else {
		if (GetFlyStatus() == 0) { SetFlyStatus(1); }
		return true; 
	}
	//std::cout << start << " " << end << " " << surface_angle << " " << ship_angle << std::endl;
}

void RigidBody::DrawFlyStatus(float dt) {
	static float t = 0;
	//static int status = 1;
	int new_status = GetFlyStatus();

	t += dt;

	if (t > 0.5) {
		t = 0;
		if (new_status != status && (status == 0 || status == 1)) {
			switch (new_status) {
			case 0: std::cout << "You are in flight" << std::endl; status_text = "You are in flight";  break;
			case 1:	std::cout << "Landing succesfull!" << std::endl; status_text = "Landing succesfull!"; break;
			case 2: std::cout << "Crash! Your velosity was to high!" << std::endl; status_text = "Crash! Your speed was to high!"; break;
			case 3: std::cout << "Crash! Your rotation speed was to high!" << std::endl; status_text = "Crash! Your rotation speed was to high!";  break;
			case 4: std::cout << "Crash! Bad landing zone!" << std::endl; status_text = "Crash! Bad landing zone!"; break;
			case 5: std::cout << "Crash! It was really bad!" << std::endl; status_text = "Crash! It was really bad!";  break;
			}	
			status = new_status;
			
		}
	}
}

void RigidBody::NOCollisionReaction() {
	if (timer.getElapsedTime().asSeconds() > 0.5) {
		SetFlyStatus(0);
	}
}