#include "pch.h"
#include "CollisionSystem.h"
#include "Core/Profile.h"
#include "Physics/Collision.h"

void sixengine::CollisionSystem::BoxWithBox(Entity entityA, Entity entityB)
{
	auto boxA = entityA.Component<BoxCollider>();
	auto boxB = entityB.Component<BoxCollider>();

	auto pos = entityA.Component<Transform>()->getWorldPosition();
	auto size = entityA.Component<BoxCollider>()->m_Size;

	float r_a = pos.x + size.x * 0.5f;
	float l_a = pos.x - size.x * 0.5f;
	float t_a = pos.y + size.y * 0.5f;
	float b_a = pos.y - size.y * 0.5f;
	float f_a = pos.z + size.z * 0.5f;
	float bc_a = pos.z - size.z * 0.5f;

	pos = entityB.Component<Transform>()->getWorldPosition();
	size = entityB.Component<BoxCollider>()->m_Size;

	float r_b = pos.x + size.x * 0.5f;
	float l_b = pos.x - size.x * 0.5f;
	float t_b = pos.y + size.y * 0.5f;
	float b_b = pos.y - size.y * 0.5f;
	float f_b = pos.z + size.z * 0.5f;
	float bc_b = pos.z - size.z * 0.5f;
	   
	if (r_a > l_b && r_b > l_a && t_a > b_b && t_b > b_a && f_a > bc_b && f_b > bc_a)
	{
		glm::vec3 totalOffset = glm::vec3();

		float left = r_a - l_b;
		float right = r_b - l_a;
		float top = t_a - b_b;
		float bottom = t_b - b_a;
		float front = f_a - bc_b;
		float back = f_b - bc_a;

		if (left > 0 && right > 0 && top > 0 && bottom > 0 && front > 0 && back > 0)
		{

		}
		else
		{
			return;
		}

		totalOffset.x = (left < right) ? (-left) : (right);
		totalOffset.y = (top < bottom) ? (-top) : (bottom);
		totalOffset.z = (back < front) ? (back) : (-front);

		if (abs(totalOffset.x) < abs(totalOffset.y) && abs(totalOffset.x) < abs(totalOffset.z)) { totalOffset.y = 0.0f; totalOffset.z = 0.0f; }
		else if (abs(totalOffset.x) > abs(totalOffset.y) && abs(totalOffset.z) > abs(totalOffset.y)) { totalOffset.x = 0.0f; totalOffset.z = 0.0f; }
		else if (abs(totalOffset.x) > abs(totalOffset.z) && abs(totalOffset.y) > abs(totalOffset.z)) { totalOffset.x = 0.0f; totalOffset.y = 0.0f; }

		float portionA = 0.5f;
		float portionB = 0.5f;

		if (boxA->isStatic() || boxB->isStatic())
		{
			if (boxA->isStatic() == false && boxB->isStatic())
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (boxA->isStatic() && boxB->isStatic() == false)
			{
				portionA = 0.0f;
				portionB = 1.0f;
			}
		}

		Collision collisionA;

		collisionA.self = entityA;
		collisionA.other = entityB;
		collisionA.offset = portionA * totalOffset;

		HandleCollision(entityA, collisionA);


		Collision collisionB;
		
		collisionB.self = entityB;
		collisionB.other = entityA;
		collisionB.offset = -portionB * totalOffset;

		HandleCollision(entityB, collisionB);
	}
}

void sixengine::CollisionSystem::SphereWithSphere(Entity entityA, Entity entityB)
{
	auto circleA = entityA.Component<SphereCollider>();
	auto circleB = entityB.Component<SphereCollider>();

	auto posA = entityA.Component<Transform>()->getWorldPosition();
	auto posB = entityB.Component<Transform>()->getWorldPosition();


	if (glm::length(posA - posB) < circleA->m_radius + circleB->m_radius)
	{
		float portionA = 0.5f;
		float portionB = 0.5f;

		if (circleA->isStatic() || circleB->isStatic())
		{
			if (circleA->isStatic() == false && circleB->isStatic())
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (circleA->isStatic() && circleB->isStatic() == false)
			{
				portionA = 0.0f;
				portionB = 1.0f;
			}
		}

		glm::vec3 totalOffset = glm::normalize(posA - posB);
		totalOffset *= (circleA->m_radius + circleB->m_radius - glm::length(posA - posB));

		Collision collisionA = Collision();

		collisionA.self = entityA;
		collisionA.other = entityB;
		collisionA.offset = portionA * totalOffset;

		HandleCollision(entityA, collisionA);

		Collision collisionB = Collision();

		collisionB.self = entityB;
		collisionB.other = entityA;
		collisionB.offset = -portionB * totalOffset;
	
		HandleCollision(entityB, collisionB);
	}
}

void sixengine::CollisionSystem::BoxWithCircle(Entity entityBox, Entity entityCircle)
{
	auto sphere = entityCircle.Component<SphereCollider>();
	glm::vec3 cirsclePos = entityCircle.Component<Transform>()->getWorldPosition();
	
	auto box = entityBox.Component<BoxCollider>();
	glm::vec3 boxPos = entityBox.Component<Transform>()->getWorldPosition();

	glm::vec3 closestPoint;
	closestPoint.x = std::clamp(cirsclePos.x, boxPos.x - box->m_Size.x * 0.5f, boxPos.x + box->m_Size.x * 0.5f);
	closestPoint.y = std::clamp(cirsclePos.y, boxPos.y - box->m_Size.y * 0.5f, boxPos.y + box->m_Size.y * 0.5f);
	closestPoint.z = std::clamp(cirsclePos.z, boxPos.z - box->m_Size.y * 0.5f, boxPos.z + box->m_Size.y * 0.5f);

	if (glm::length(cirsclePos - closestPoint) < sphere->m_radius)
	{
		glm::vec3 totalOffset = glm::vec3();

		if (closestPoint == cirsclePos)
		{
			float r = boxPos.x + box->m_Size.x * 0.5f;
			float l = boxPos.x - box->m_Size.x * 0.5f;
			float t = boxPos.y + box->m_Size.y * 0.5f;
			float b = boxPos.y - box->m_Size.y * 0.5f;
			float f = boxPos.z + box->m_Size.z * 0.5f;
			float bc = boxPos.z - box->m_Size.z * 0.5f;

			float c_x = cirsclePos.x;
			float c_y = cirsclePos.y;
			float c_z = cirsclePos.z;
			float ro = sphere->m_radius;


			float left = -c_x - l + ro;
			float right = r - c_x + ro;
			float top = c_y - t + ro;
			float bottom = b - c_y + ro;
			float front = f - c_z + ro;
			float back = -c_z - b + ro;

			totalOffset.x = (left < right) ? (-left) : (right);
			totalOffset.y = (top < bottom) ? (-top) : (bottom);
			totalOffset.z = (back < front) ? (-back) : (front);

			if (abs(totalOffset.x) < abs(totalOffset.y) && abs(totalOffset.x) < abs(totalOffset.z)) { totalOffset.y = 0.0f; totalOffset.z = 0.0f; }
			else if (abs(totalOffset.x) > abs(totalOffset.y) && abs(totalOffset.z) > abs(totalOffset.y)) { totalOffset.x = 0.0f; totalOffset.z = 0.0f; }
			else if (abs(totalOffset.x) > abs(totalOffset.z) && abs(totalOffset.y) > abs(totalOffset.z)) { totalOffset.x = 0.0f; totalOffset.y = 0.0f; }
		}
		else
		{
			totalOffset = glm::normalize(cirsclePos - closestPoint);
			totalOffset *= (sphere->m_radius - glm::length(cirsclePos - closestPoint));
		}


		float portionA = 0.5f;
		float portionB = 0.5f;

		if (box->isStatic() || sphere->isStatic())
		{
			if (box->isStatic() && sphere->isStatic())
			{
				portionA = 0.0f;
				portionB = 0.0f;
			}
			else if (box->isStatic() == false && sphere->isStatic())
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (box->isStatic() && sphere->isStatic() == false)
			{
				portionA = 0.0f;
				portionB = 1.0f;
			}
		}

		Collision collisionA = Collision();

		collisionA.self = entityCircle;
		collisionA.other = entityBox;
		collisionA.offset = portionB * totalOffset;

		HandleCollision(entityCircle, collisionA);

		Collision collisionB = Collision();

		collisionB.self = entityBox;
		collisionB.other = entityCircle;
		collisionB.offset = -portionA * totalOffset;
		HandleCollision(entityBox, collisionB);
	}
}

void sixengine::CollisionSystem::UpdateBoxesWithBoxes()
{
	auto boxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();

	for (int i = 0; i < boxes.size(); i++)
	{
		for (int j = i + 1; j < boxes.size(); j++)
		{
			auto entityA = boxes[i];
			auto entityB = boxes[j];

			BoxWithBox(entityA, entityB);
		}
	}
}

void sixengine::CollisionSystem::UpdateSpheresWithSpheres()
{
	auto spheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

	for (int i = 0; i < spheres.size(); i++)
	{
		for (int j = i + 1; j < spheres.size(); j++)
		{
			auto entityA = spheres[i];
			auto entityB = spheres[j];

			SphereWithSphere(entityA, entityB);
		}
	}
}

void sixengine::CollisionSystem::UpdateBoxesWithSpheres()
{
	auto boxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();
	auto spheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

	for (int i = 0; i < boxes.size(); i++)
	{
		for (int j = 0; j < spheres.size(); j++)
		{
			auto box = boxes[i];
			auto circle = spheres[j];

			BoxWithCircle(box, circle);
		}
	}
}

void sixengine::CollisionSystem::HandleCollision(Entity collider, Collision collision)
{
	auto t = collider.Component<Transform>();
	auto world = t->GetWorld();
	world = glm::translate(world, collision.offset);

	t->SetWorld(world);
}

void sixengine::CollisionSystem::UpdateAll(EventManager& eventManager, float dt)
{
	UpdateBoxesWithBoxes();
	UpdateSpheresWithSpheres();
	UpdateBoxesWithSpheres();
}
