#include "pch.h"
#include "CollisionSystem.h"
#include "Core/Profile.h"
#include "Physics/Collision.h"

#include "Gameplay/Components/Transform.h"

#include "glm/gtx/string_cast.hpp"

#include <algorithm>
#include <glm/gtx/projection.hpp>

#undef min
#undef max

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
	float f_a = pos.z + size.x * 0.5f;
	float bc_a = pos.z - size.z * 0.5f;

	pos = entityB.Component<Transform>()->getWorldPosition();
	size = entityB.Component<BoxCollider>()->m_Size;

	float r_b = pos.x + size.x * 0.5f;
	float l_b = pos.x - size.x * 0.5f;
	float t_b = pos.y + size.y * 0.5f;
	float b_b = pos.y - size.y * 0.5f;
	float f_b = pos.z + size.x * 0.5f;
	float bc_b = pos.z - size.z * 0.5f;

	if (r_a > l_b && r_b > l_a && b_a < t_b && b_b < t_a && f_a > bc_b && f_b > bc_a)
	{
		glm::vec3 totalOffset = glm::vec3(0.0f, 0.0f, 0.0f);

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
		totalOffset.z = (front < back) ? (-front) : (back);

		float min = std::min(std::min(abs(totalOffset.x), abs(totalOffset.y)), abs(totalOffset.z));

		if (abs(totalOffset.x) > min) { totalOffset.x = 0.0f; }
		if (abs(totalOffset.y) > min) { totalOffset.y = 0.0f; }
		if (abs(totalOffset.z) > min) { totalOffset.z = 0.0f; }

		float portionA = 0.5f;
		float portionB = 0.5f;

		if (entityA.HasComponent<DynamicBody>() && entityB.HasComponent<DynamicBody>())
		{
		}
		else
		{
			if (entityA.HasComponent<DynamicBody>() && entityA.HasComponent<DynamicBody>() == false)
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (entityA.HasComponent<DynamicBody>() == false && entityA.HasComponent<DynamicBody>())
			{
				portionA = 0.0f;
				portionB = 1.0f;
			}
		}

		Collision collisionA = Collision();

		collisionA.self = entityA;
		collisionA.other = entityB;
		collisionA.offset = portionA * totalOffset;
		entityA.Component<Transform>()->Translate(portionA * totalOffset);

		HandleCollision(entityA, collisionA);


		Collision collisionB = Collision();

		collisionB.self = entityB;
		collisionB.other = entityA;
		collisionB.offset = -portionB * totalOffset;
		entityB.Component<Transform>()->Translate(-portionB * totalOffset);

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

		if (entityA.HasComponent<DynamicBody>() && entityB.HasComponent<DynamicBody>())
		{
		}
		else
		{
			if (entityA.HasComponent<DynamicBody>() && entityA.HasComponent<DynamicBody>() == false)
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (entityA.HasComponent<DynamicBody>() == false && entityA.HasComponent<DynamicBody>())
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
		entityA.Component<Transform>()->Translate(portionA * totalOffset);

		HandleCollision(entityA, collisionA);

		Collision collisionB = Collision();

		collisionB.self = entityB;
		collisionB.other = entityA;
		collisionB.offset = -portionB * totalOffset;
		entityB.Component<Transform>()->Translate(-portionB * totalOffset);

		HandleCollision(entityB, collisionB);
	}
}

void sixengine::CollisionSystem::BoxWithCircle(Entity entityBox, Entity entityCircle)
{
	Transform* boxTransform = entityBox.Component<Transform>().Get();
	BoxCollider* boxCollider = entityBox.Component<BoxCollider>().Get();
	glm::vec3 boxPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	

	Transform* sphereTransform = entityCircle.Component<Transform>().Get();
	SphereCollider* sphereCollider = entityCircle.Component<SphereCollider>().Get();
	glm::vec3 spherePosition = boxTransform->InverseTransformPoint(sphereTransform->GetWorldPosition());

	glm::vec3 closestPoint;
	closestPoint.x = std::clamp(spherePosition.x, boxPosition.x - boxCollider->m_Size.x * 0.5f, boxPosition.x + boxCollider->m_Size.x * 0.5f);
	closestPoint.y = std::clamp(spherePosition.y, boxPosition.y - boxCollider->m_Size.y * 0.5f, boxPosition.y + boxCollider->m_Size.y * 0.5f);
	closestPoint.z = std::clamp(spherePosition.z, boxPosition.z - boxCollider->m_Size.z * 0.5f, boxPosition.z + boxCollider->m_Size.z * 0.5f);

	if (glm::length(spherePosition - closestPoint) < sphereCollider->m_radius)
	{
		glm::vec3 totalOffset = glm::vec3();

		if (closestPoint == spherePosition)
		{
			float r = boxPosition.x + boxCollider->m_Size.x * 0.5f;
			float l = boxPosition.x - boxCollider->m_Size.x * 0.5f;
			float t = boxPosition.y + boxCollider->m_Size.y * 0.5f;
			float b = boxPosition.y - boxCollider->m_Size.y * 0.5f;
			float f = boxPosition.z + boxCollider->m_Size.z * 0.5f;
			float bc = boxPosition.z - boxCollider->m_Size.z * 0.5f;

			float c_x = spherePosition.x;
			float c_y = spherePosition.y;
			float c_z = spherePosition.z;
			float ro = sphereCollider->m_radius;

			float left = -c_x - l + ro;
			float right = r - c_x + ro;
			float top = c_y - t + ro;
			float bottom = b - c_y + ro;
			float front = -c_z - f + ro;
			float back = bc - c_z + ro;

			totalOffset.x = (left < right) ? (-left) : (right);
			totalOffset.y = (top < bottom) ? (-top) : (bottom);
			totalOffset.z = (front < back) ? (-front) : (back);

			if (abs(totalOffset.x) == 0.0f) { totalOffset.x = FLT_MAX; }
			if (abs(totalOffset.y) == 0.0f) { totalOffset.y = FLT_MAX; }
			if (abs(totalOffset.z) == 0.0f) { totalOffset.z = FLT_MAX; }

			float min = std::min(std::min(abs(totalOffset.x), abs(totalOffset.y)), abs(totalOffset.z));

			if (abs(totalOffset.x) > min) { totalOffset.x = 0.0f; }
			if (abs(totalOffset.y) > min) { totalOffset.y = 0.0f; }
			if (abs(totalOffset.z) > min) { totalOffset.z = 0.0f; }
		}
		else
		{
			totalOffset = glm::normalize(spherePosition - closestPoint);
			totalOffset *= (sphereCollider->m_radius - glm::length(spherePosition - closestPoint));
		}


		float portionA = 0.5f;
		float portionB = 0.5f;

		if (entityBox.HasComponent<DynamicBody>() && entityCircle.HasComponent<DynamicBody>())
		{
		}
		else
		{
			if (entityBox.HasComponent<DynamicBody>() && entityCircle.HasComponent<DynamicBody>() == false)
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (entityBox.HasComponent<DynamicBody>() == false && entityCircle.HasComponent<DynamicBody>())
			{
				portionA = 0.0f;
				portionB = 1.0f;
			}
		}

		Collision collisionB = Collision();

		collisionB.self = entityBox;
		collisionB.other = entityCircle;
		collisionB.offset = -portionA * (boxTransform->TransformPoint(totalOffset) - boxTransform->GetWorldPosition());
		HandleCollision(entityBox, collisionB);
		boxTransform->SetWorldPosition(boxTransform->TransformPoint(-portionA * totalOffset));

		Collision collisionA = Collision();

		collisionA.self = entityCircle;
		collisionA.other = entityBox;
		collisionA.offset = portionB * (boxTransform->TransformPoint(spherePosition + totalOffset) - sphereTransform->GetWorldPosition());
		sphereTransform->SetWorldPosition(boxTransform->TransformPoint(portionB * spherePosition + totalOffset));

		HandleCollision(entityCircle, collisionA);
	}
}

void sixengine::CollisionSystem::UpdateBoxesWithBoxes()
{
	auto dynamicBoxes = Application::Get().GetEntityManager()->EntitiesWithComponents<DynamicBody, BoxCollider>();
	auto allBoxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();

	for (int i = 0; i < dynamicBoxes.size(); i++)
	{
		for (int j = i; j < dynamicBoxes.size(); j++)
		{
			auto entityA = dynamicBoxes[i];
			auto entityB = allBoxes[j];

			BoxWithBox(entityA, entityB);
		}

		for (int j = 0; j < allBoxes.size(); j++)
		{
			if (allBoxes[j].HasComponent<DynamicBody>())
			{
				continue;
			}

			auto entityA = dynamicBoxes[i];
			auto entityB = allBoxes[j];

			BoxWithBox(entityA, entityB);
		}
	}
}

void sixengine::CollisionSystem::UpdateSpheresWithSpheres()
{
	auto dynamicSpheres = Application::Get().GetEntityManager()->EntitiesWithComponents<DynamicBody, SphereCollider>();
	auto allSpheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

	for (int i = 0; i < dynamicSpheres.size(); i++)
	{
		for (int j = i + 1; j < dynamicSpheres.size(); j++)
		{
			auto entityA = dynamicSpheres[i];
			auto entityB = dynamicSpheres[j];

			SphereWithSphere(entityA, entityB);
		}

		for (int j = 0; j < allSpheres.size(); j++)
		{
			if (allSpheres[j].HasComponent<DynamicBody>()) { continue; }

			auto entityA = dynamicSpheres[i];
			auto entityB = allSpheres[j];

			SphereWithSphere(entityA, entityB);
		}
	}
}

void sixengine::CollisionSystem::UpdateBoxesWithSpheres()
{
	auto dynamicBoxes = Application::Get().GetEntityManager()->EntitiesWithComponents<DynamicBody, BoxCollider>();
	auto allBoxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();
	
	auto dybamicSpheres = Application::Get().GetEntityManager()->EntitiesWithComponents<DynamicBody, SphereCollider>();
	auto allSpheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

	for (int i = 0; i < dynamicBoxes.size(); i++)
	{
		for (int j = 0; j < allSpheres.size(); j++)
		{
			auto box = dynamicBoxes[i];
			auto circle = allSpheres[j];

			BoxWithCircle(box, circle);
		}
	}

	for (int i = 0; i < dybamicSpheres.size(); i++)
	{
		for (int j = 0; j < allBoxes.size(); j++)
		{
			auto sphere = dybamicSpheres[i];
			auto box = allBoxes[j];

			BoxWithCircle(box, sphere);
		}
	}
}

void sixengine::CollisionSystem::HandleCollision(Entity collider, Collision collision)
{
	m_EventManager->Emit<OnCollision>(OnCollision(collider, collision));
}

void sixengine::CollisionSystem::UpdateAll(EventManager& eventManager, float dt)
{
	PROFILE_SCOPE("Collision Update");

	m_EventManager = &eventManager;

	UpdateBoxesWithBoxes();
	UpdateSpheresWithSpheres();
	UpdateBoxesWithSpheres();
}

glm::vec3 sixengine::CollisionSystem::ClosestPointOnSphere(glm::vec3 spherePos, float sphereRad, glm::vec3 point)
{
	// First, get a vetor from the sphere to the point
	glm::vec3 sphereToPoint = point - spherePos;
	// Normalize that vector
	sphereToPoint = glm::normalize(sphereToPoint);
	// Adjust it's length to point to edge of sphere
	sphereToPoint *= sphereRad;
	// Translate into world space
	glm::vec3 worldPoint = spherePos + sphereToPoint;
	// Return new point
	return glm::vec3(worldPoint.x, worldPoint.y, worldPoint.z);
}

// Test if ray r = p + td intersects sphere s
int sixengine::CollisionSystem::TestRaySphere(glm::vec3 p, glm::vec3 d, Entity entity)
{
	//float t = 0.0f;
	//glm::vec3 q = glm::vec3(0.0f);

	//glm::vec3 m = p - entity.Component<Transform>()->GetWorldPosition();
	//float b = glm::dot(m, d);
	//float c = glm::dot(m, m) - entity.Component<SphereCollider>()->m_radius * entity.Component<SphereCollider>()->m_radius;
	//// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
	//if (c > 0.0f && b > 0.0f) return 0;
	//float discr = b * b - c;
	//// A negative discriminant corresponds to ray missing sphere
	//if (discr < 0.0f) return 0;
	//// Ray now found to intersect sphere, compute smallest t value of intersection
	//t = -b - glm::sqrt(discr);
	//// If t is negative, ray started inside sphere so clamp t to zero
	//if (t < 0.0f) t = 0.0f;
	//q = p + t * d;

	//return 1;

	return 0;
}

int sixengine::CollisionSystem::TestSegmentSphere(glm::vec3 v, glm::vec3 w, Entity entity)
{
	glm::vec3 p = entity.Component<Transform>()->GetWorldPosition();

	// Return minimum distance between line segment vw and point p
	const float l2 = glm::distance2(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
	if (l2 == 0.0) return distance(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.
	const float t = std::max(0.0f, std::min(1.0f, dot(p - v, w - v) / l2));
	const glm::vec3 projection = v + t * (w - v);  // Projection falls on the segment
	return glm::distance(p, projection) < entity.Component<SphereCollider>()->m_radius;
}

// Test if segment specified by points p0 and p1 intersects AABB b
int sixengine::CollisionSystem::TestSegmentAABB(glm::vec3 p0, glm::vec3 p1, Entity entity)
{
	p0 = entity.Component<Transform>()->InverseTransformPoint(p0);
	p1 = entity.Component<Transform>()->InverseTransformPoint(p1);

	BoxCollider* box = entity.Component<BoxCollider>().Get();

	glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0); // Box center-point
	glm::vec3 e = box->m_Size * 0.5f; // Box halflength extents
	glm::vec3 m = (p0 + p1) * 0.5f; // Segment midpoint
	glm::vec3 d = p1 - m; // Segment halflength vector
	m = m - c; // Translate box and segment to origin

	// Try world coordinate axes as separating axes
	float adx = abs(d.x);
	if (abs(m.x) > e.x + adx) return 0;
	float ady = abs(d.y);
	if (abs(m.y) > e.y + ady) return 0;
	float adz = abs(d.z);
	if (abs(m.z) > e.z + adz) return 0;
	// Add in an epsilon term to counteract arithmetic errors when segment is
	// (near) parallel to a coordinate axis (see text for detail)
	adx += 0.001f; ady += 0.001f; adz += 0.001f;

	// Try cross products of segment direction vector with coordinate axes
	if (abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	if (abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return 0;
	if (abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return 0;
	// No separating axis found; segment must be overlapping AABB

	return 1;
}

std::vector<Entity> sixengine::CollisionSystem::RaycastAll(glm::vec3 origin, glm::vec3 direction)
{
	std::vector<Entity> result;

	float distance = glm::dot(direction, direction);

	{
		auto boxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();

		for (int i = 0; i < boxes.size(); i++)
		{
			if (glm::distance2(boxes[i].Component<Transform>()->GetWorldPosition(), origin) > distance)
			{
				continue;
			}

			if (TestSegmentAABB(origin, origin + direction, boxes[i]))
			{
				result.push_back(boxes[i]);
			}
		}
	}

	{
		auto spheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

		for (int i = 0; i < spheres.size(); i++)
		{
			if (glm::distance2(spheres[i].Component<Transform>()->GetWorldPosition(), origin) > distance + spheres[i].Component<SphereCollider>()->m_radius)
			{
				continue;
			}

			if (TestSegmentSphere(origin, origin + direction, spheres[i]))
			{
				result.push_back(spheres[i]);
			}
		}
	}

	return result;
}

Entity sixengine::CollisionSystem::CheckSphere(glm::vec3 center, float radius)
{
	{
		auto spheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

		for (int i = 0; i < spheres.size(); i++)
		{
			auto other = spheres[i].Component<SphereCollider>();
			auto otherPos = spheres[i].Component<Transform>()->GetWorldPosition();


			if (glm::length(center - otherPos) < radius + other->m_radius)
			{
				float portionA = 0.5f;
				float portionB = 0.5f;

				LOG_ERROR("SYSTEM HIT COLLIDER");
				return spheres[i];

			}
		}
	}

	{
		auto boxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();

		for (int i = 0; i < boxes.size(); i++)
		{

			auto box = boxes[i].Component<BoxCollider>();
			glm::vec3 relativeCenter = boxes[i].Component<Transform>()->InverseTransformPoint(center);

			glm::vec3 closestPoint;
			closestPoint.x = std::clamp(relativeCenter.x, -box->m_Size.x * 0.5f, box->m_Size.x * 0.5f);
			closestPoint.y = std::clamp(relativeCenter.y, -box->m_Size.y * 0.5f, box->m_Size.y * 0.5f);
			closestPoint.z = std::clamp(relativeCenter.z, -box->m_Size.z * 0.5f, box->m_Size.z * 0.5f);

			if (glm::length(relativeCenter - closestPoint) < radius)
			{
				LOG_ERROR("SYSTEM HIT COLLIDER");
				return boxes[i];
			}
		}
	}

	return Entity();	
}

std::vector<Entity> sixengine::CollisionSystem::CheckSphereAll(glm::vec3 center, float radius)
{
	std::vector<Entity> result;

	{
		auto spheres = Application::Get().GetEntityManager()->EntitiesWithComponents<SphereCollider>();

		for (int i = 0; i < spheres.size(); i++)
		{
			auto other = spheres[i].Component<SphereCollider>();
			auto otherPos = spheres[i].Component<Transform>()->GetWorldPosition();


			if (glm::length(center - otherPos) < radius + other->m_radius)
			{
				float portionA = 0.5f;
				float portionB = 0.5f;

				result.push_back(spheres[i]);
			}
		}
	}

	{
		auto boxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();

		for (int i = 0; i < boxes.size(); i++)
		{
			auto box = boxes[i].Component<BoxCollider>();
			glm::vec3 relativeCenter = boxes[i].Component<Transform>()->InverseTransformPoint(center);

			glm::vec3 closestPoint;
			closestPoint.x = std::clamp(relativeCenter.x, -box->m_Size.x * 0.5f, box->m_Size.x * 0.5f);
			closestPoint.y = std::clamp(relativeCenter.y, -box->m_Size.y * 0.5f, box->m_Size.y * 0.5f);
			closestPoint.z = std::clamp(relativeCenter.z, -box->m_Size.z * 0.5f, box->m_Size.z * 0.5f);

			if (glm::length(relativeCenter - closestPoint) < radius)
			{
				result.push_back(boxes[i]);
			}
		}
	}

	return result;
}
