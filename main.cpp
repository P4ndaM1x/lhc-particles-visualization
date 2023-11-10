#include "pch.h"

struct ParticleProperties{
	float phi;
	float pt;
	float q;
};

std::vector<std::pair<unsigned, sf::Vector3f>> spacepoints;
std::unordered_map<unsigned, ParticleProperties> properties;

struct Spherical{
	float distance, theta, phi;
	Spherical(float gdistance, float gtheta, float gphi) : distance(gdistance), theta(gtheta), phi(gphi){}
	float getX(){
		return distance * cos(phi) * cos(theta);
	}
	float getY(){
		return distance * sin(phi);
	}
	float getZ(){
		return distance * cos(phi) * sin(theta);
	}
};

Spherical camera(3.0f, 1.0f, 0.2f), light_position(4.0f, 0.2f, 1.2f);
sf::Vector3f pos(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), rot(0.0f, 0.0f, 0.0f);
bool perspective_projection = true;
float fov = 45.0f;

float* pos_offsets[3] = { &pos.x, &pos.y, &pos.z };
float* scale_offsets[3] = { &scale.x, &scale.y, &scale.z };
float* rot_offsets[3] = { &rot.x, &rot.y, &rot.z };

void initOpenGL(void){
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
}

void reshapeScreen(sf::Vector2u size){
	glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if( perspective_projection ) gluPerspective(fov, (GLdouble)size.x / (GLdouble)size.y, 0.1, 100.0);
	else glOrtho(-1.245 * ((GLdouble)size.x / (GLdouble)size.y), 1.245 * ((GLdouble)size.x / (GLdouble)size.y), -1.245, 1.245, -3.0, 12.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void drawScene(){
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	Spherical north_of_camera(camera.distance, camera.theta, camera.phi + 0.01f);
	gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
		0.0, 0.0, 0.0,
		north_of_camera.getX(), north_of_camera.getY(), north_of_camera.getZ());
	GLfloat light0_position[4] = { light_position.getX(), light_position.getY(), light_position.getZ(), 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	//uklad
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
	glColor3f(0.0, 1.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(0, 1.0, 0);
	glColor3f(0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
	glEnd();

	//Linie przerywane
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0xAAAA);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(-1.0, 0, 0);
	glColor3f(0.0, 1.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(0, -1.0, 0);
	glColor3f(0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f(0, 0, -1.0);
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	//transformacje
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rot.x, 1, 0, 0);
	glRotatef(rot.y, 0, 1, 0);
	glRotatef(rot.z, 0, 0, 1);
	glScalef(scale.x, scale.y, scale.z);

	glBegin(GL_POINTS);
	//glVertex3f(point.x, point.y, point].z);
	glEnd();
}


int main(){
	#define SPACEPOINTS_FILEPATH "spacepoints.csv"
	const size_t spacepoints_len = csv::get_file_info(SPACEPOINTS_FILEPATH).n_rows;
	spacepoints.reserve(spacepoints_len);

	csv::CSVReader spacepoints_reader(SPACEPOINTS_FILEPATH);
	for( csv::CSVRow& row: spacepoints_reader ){
		spacepoints.push_back(std::make_pair(row["event_id"].get<unsigned>(), sf::Vector3f{ row["x"].get<float>() , row["y"].get<float>() , row["z"].get<float>() }));
	}

	#define PROPERTIES_FILEPATH "particles_initial.csv"
	const size_t properties_len = csv::get_file_info(PROPERTIES_FILEPATH).n_rows;
	properties.reserve(properties_len);

	csv::CSVReader properties_reader(PROPERTIES_FILEPATH);
	for( csv::CSVRow& row: properties_reader ){
		properties[row["event_id"].get<unsigned>()] = ParticleProperties{ row["phi"].get<float>(), row["pt"].get<float>(), row["q"].get<float>() };
	}

	bool running = true;
	sf::ContextSettings context(24, 0, 0, 4, 5);
	sf::RenderWindow window(sf::VideoMode(1280, 1024), "Open GL Lab1 04", 7U, context);
	sf::Clock deltaClock;

	ImGui::SFML::Init(window);

	window.setVerticalSyncEnabled(true);

	reshapeScreen(window.getSize());
	initOpenGL();

	while( running ){
		sf::Event event;
		while( window.pollEvent(event) ){
			ImGui::SFML::ProcessEvent(event);
			if( event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) ) running = false;
			if( event.type == sf::Event::Resized ) reshapeScreen(window.getSize());
		}

		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ){
			camera.theta -= 0.0174533f; if( camera.theta < 0.0f ) camera.theta = 0.0f;
		}
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ){
			camera.theta += 0.0174533f; if( camera.theta >(float)(2.0*std::numbers::pi) ) camera.theta = (float)(2.0*std::numbers::pi);
		}
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ){
			camera.phi += 0.0174533f;  if( camera.phi > (float)std::numbers::pi ) camera.phi = (float)std::numbers::pi;
		}
		if( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ){
			camera.phi -= 0.0174533f; if( camera.phi < 0.0f ) camera.phi = 0.0f;
		}

		drawScene();

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Camera");
		ImGui::SliderFloat("R", &camera.distance, 0.5f, 10.0f);
		ImGui::SliderAngle("theta", &camera.theta, 0.0f, 360.0f);
		ImGui::SliderAngle("phi", &camera.phi, 0.0f, 180.0f);
		if( ImGui::Checkbox("Perspective projection", &perspective_projection) ) reshapeScreen(window.getSize());
		if( ImGui::SliderFloat("FoV", &fov, 10.0f, 90.0f) ) reshapeScreen(window.getSize());
		ImGui::End();

		ImGui::Begin("Transformations");
		ImGui::SliderFloat3("Position", *pos_offsets, -3.0f, 3.0f);
		ImGui::SliderFloat3("Scale", *scale_offsets, -2.0f, 2.0f);
		ImGui::SliderFloat3("Rotation", *rot_offsets, -180.0f, 180.0f);
		ImGui::End();

		ImGui::Begin("Light position");
		ImGui::SliderAngle("theta", &light_position.theta, 0.0f, 360.0f);
		ImGui::SliderAngle("phi", &light_position.phi, 0.0f, 180.0f);
		ImGui::End();

		ImGui::SFML::Render(window);

		window.display();
	}
	ImGui::SFML::Shutdown();
	return 0;
}

