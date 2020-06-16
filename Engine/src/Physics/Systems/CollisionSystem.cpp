#include "pch.h"
#include "CollisionSystem.h"
#include "Core/Profile.h"
#include "Physics/Collision.h"

#include "Gameplay/Components/Transform.h"

#include "glm/gtx/string_cast.hpp"

#include <algorithm>

#undef min
#undef max

// region R = { (x, y, z) | min.x<=x<=max.x, min.y<=y<=max.y, min.z<=z<=max.z }
struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	AABB(sixengine::Transform* t, sixengine::BoxCollider* box)
	{
		glm::vec3 p0 = t->TransformPoint(glm::vec3(box->m_Size.x * 0.5f, box->m_Size.y * 0.5f, box->m_Size.z * 0.5f));
		glm::vec3 p1 = t->TransformPoint(glm::vec3(-box->m_Size.x * 0.5f, box->m_Size.y * 0.5f, box->m_Size.z * 0.5f));
		glm::vec3 p2 = t->TransformPoint(glm::vec3(box->m_Size.x * 0.5f, -box->m_Size.y * 0.5f, box->m_Size.z * 0.5f));
		glm::vec3 p3 = t->TransformPoint(glm::vec3(-box->m_Size.x * 0.5f, -box->m_Size.y * 0.5f, box->m_Size.z * 0.5f));
		glm::vec3 p4 = t->TransformPoint(glm::vec3(box->m_Size.x * 0.5f, box->m_Size.y * 0.5f, -box->m_Size.z * 0.5f));
		glm::vec3 p5 = t->TransformPoint(glm::vec3(-box->m_Size.x * 0.5f, box->m_Size.y * 0.5f, -box->m_Size.z * 0.5f));
		glm::vec3 p6 = t->TransformPoint(glm::vec3(box->m_Size.x * 0.5f, -box->m_Size.y * 0.5f, -box->m_Size.z * 0.5f));
		glm::vec3 p7 = t->TransformPoint(glm::vec3(-box->m_Size.x * 0.5f, -box->m_Size.y * 0.5f, -box->m_Size.z * 0.5f));

		min = glm::vec3(0.0f);
		max = glm::vec3(0.0f);

		min.x = std::min({ p0.x, p1.x, p2.x, p3.x, p4.x, p5.x, p6.x, p7.x });
		min.y = std::min({ p0.y, p1.y, p2.y, p3.y, p4.y, p5.y, p6.y, p7.y });
		min.z = std::min({ p0.z, p1.z, p2.z, p3.z, p4.z, p5.z, p6.z, p7.z });

		max.x = std::max({ p0.x, p1.x, p2.x, p3.x, p4.x, p5.x, p6.x, p7.x });
		max.y = std::max({ p0.y, p1.y, p2.y, p3.y, p4.y, p5.y, p6.y, p7.y });
		max.z = std::max({ p0.z, p1.z, p2.z, p3.z, p4.z, p5.z, p6.z, p7.z });
	}

};

// Region R = { x|x= c+r*u[0]+s*u[1]+t*u[2] } , |r|<=e[0], |s|<=e[1], |t|<=e[2]
struct OBB {
	glm::vec3 c; // OBB center point
	glm::vec3 u[3]; // Local x-, y-, and z-axes
	glm::vec3 e; // Positive halfwidth extents of OBB along each axis
};

// Intersect ray R(t) = p + t*d against AABB a. When intersecting,
// return intersection distance tmin and point q of intersection
int IntersectRayAABB(glm::vec3 p, glm::vec3 d, AABB a, float& tmin, glm::vec3& q)
{
	tmin = 0.0f; // set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

	// For all three slabs
	for (int i = 0; i < 3; i++) {
		if (glm::abs(d[i]) < 0.001f) {
			// Ray is parallel to slab. No hit if origin not within slab
			if (p[i] < a.min[i] || p[i] > a.max[i]) return 0;
		}
		else {
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / d[i];
			float t1 = (a.min[i] - p[i]) * ood;
			float t2 = (a.max[i] - p[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return 0;
		}
	}

	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	q = p + d * tmin;
	return 1;
}

static int sixengine::RayCast(glm::vec3 p, glm::vec3 d)
{
	auto boxes = Application::Get().GetEntityManager()->EntitiesWithComponents<BoxCollider>();

	for (auto b : boxes)
	{
		float f;
		glm::vec3 v;
		if (IntersectRayAABB(p, d, AABB(b.Component<Transform>().Get(), b.Component<BoxCollider>().Get()), f, v))
		{
			return 1;
		}
	}

	return 0;
}

float minimini(glm::vec3 v)
{
	// absolut non zero minimum
	float m = INFINITY;

	for (int i = 0; i < 3; i++)
	{
		if (v[i] != 0 && glm::abs(v[i]) < m)
		{
			m = glm::abs(v[i]);
		}
	}

	if (m == INFINITY)
	{
		m = 0.0f;
	}

	return m;
}

int TestOBBOBB(OBB& a, OBB& b, glm::vec3& offset)
{
	float ra, rb;
	glm::mat3 R, absR;
	// Compute rotation matrix expressing b in a�s coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = glm::dot(a.u[i], b.u[j]);

	// Compute translation vector t
	glm::vec3 t = b.c - a.c;
	// Bring translation into a�s coordinate frame
	t = glm::vec3(glm::dot(t, a.u[0]), glm::dot(t, a.u[1]), glm::dot(t, a.u[2]));

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			absR[i][j] = glm::abs(R[i][j]) + 0.0001f;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {
		ra = a.e[i];
		rb = b.e[0] * absR[i][0] + b.e[1] * absR[i][1] + b.e[2] * absR[i][2];
		if (glm::abs(t[i]) > ra + rb) return 0;
		float X = t[i];

		//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X));
		offset[i] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);
	}
	
	LOG_INFO("");

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = a.e[0] * absR[0][i] + a.e[1] * absR[1][i] + a.e[2] * absR[2][i];
		rb = b.e[i];
		if (glm::abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return 0;
		float X = t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i];
		
		//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X));
		if (offset[i] > glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X))
			offset[i] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);
	}

	LOG_INFO("");

	// Test axis L = A0 x B0
	ra = a.e[1] * absR[2][0] + a.e[2] * absR[1][0];
	rb = b.e[1] * absR[0][2] + b.e[2] * absR[0][1];
	if (glm::abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return 0;
	float X = t[2] * R[1][0] - t[1] * R[2][0];
	LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - glm::abs(X)));

	// Test axis L = A0 x B1
	ra = a.e[1] * absR[2][1] + a.e[2] * absR[1][1];
	rb = b.e[0] * absR[0][2] + b.e[2] * absR[0][0];
	if (glm::abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return 0;
	X = t[2] * R[1][1] - t[1] * R[2][1];
	//LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - glm::abs(X)));
	if (glm::abs(offset[2]) > (glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X)))
		offset[2] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);

	// Test axis L = A0 x B2
	ra = a.e[1] * absR[2][2] + a.e[2] * absR[1][2];
	rb = b.e[0] * absR[0][1] + b.e[1] * absR[0][0];
	if (glm::abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return 0;
	X = t[2] * R[1][2] - t[1] * R[2][2];
	//LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - glm::abs(X)));
	if (glm::abs(offset[1]) > (glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X)))
		offset[1] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);


	// Test axis L = A1 x B0
	ra = a.e[0] * absR[2][0] + a.e[2] * absR[0][0];
	rb = b.e[1] * absR[1][2] + b.e[2] * absR[1][1];
	if (glm::abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return 0;
	X = t[0] * R[2][0] - t[2] * R[0][0];
	//LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - glm::abs(X)));
	if (glm::abs(offset[2]) > (glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X)))
		offset[2] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);

	// Test axis L = A1 x B1
	ra = a.e[0] * absR[2][1] + a.e[2] * absR[0][1];
	rb = b.e[0] * absR[1][2] + b.e[2] * absR[1][0];
	if (glm::abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return 0;
	X = t[0] * R[2][1] - t[2] * R[0][1];
	LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - glm::abs(X)));

	// Test axis L = A1 x B2
	ra = a.e[0] * absR[2][2] + a.e[2] * absR[0][2];
	rb = b.e[0] * absR[1][1] + b.e[1] * absR[1][0];
	if (glm::abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return 0;
	X = t[0] * R[2][2] - t[2] * R[0][2];
	//LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - glm::abs(X)));
	if (glm::abs(offset[0]) > (glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X)))
		offset[0] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);

	// Test axis L = A2 x B0
	ra = a.e[0] * absR[1][0] + a.e[1] * absR[0][0];
	rb = b.e[1] * absR[2][2] + b.e[2] * absR[2][1];
	if (glm::abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return 0;
	X = t[1] * R[0][0] - t[0] * R[1][0];
	//LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f))* (ra + rb - glm::abs(X)));
	if (glm::abs(offset[1]) > (glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X)))
		offset[1] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);

	// Test axis L = A2 x B1
	ra = a.e[0] * absR[1][1] + a.e[1] * absR[0][1];
	rb = b.e[0] * absR[2][2] + b.e[2] * absR[2][0];
	if (glm::abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return 0;
	X = t[1] * R[0][1] - t[0] * R[1][1];
	//LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	//LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f))* (ra + rb - glm::abs(X)));
	if (glm::abs(offset[0]) > (glm::abs((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X)))
		offset[0] = ((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f)) * (ra + rb - X);


	// Test axis L = A2 x B2
	ra = a.e[0] * absR[1][2] + a.e[1] * absR[0][2];
	rb = b.e[0] * absR[2][1] + b.e[1] * absR[2][0];
	if (glm::abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return 0;
	X = t[1] * R[0][2] - t[0] * R[1][2];
	LOG_INFO("{0}, {1}, {2}", ra, rb, X);
	LOG_INFO(((glm::sign(X) != 0) ? (glm::sign(X)) : (1.0f))* (ra + rb - glm::abs(X)));

	offset.x = -glm::sign(t.x) * glm::abs(offset.x);
	offset.y = -glm::sign(t.y) * glm::abs(offset.y);
	offset.z =  glm::sign(t.z) * glm::abs(offset.z);

	LOG_INFO(glm::to_string(offset));

	if (glm::abs(offset.x) > minimini(offset)) { offset.x = 0.0f; }
	if (glm::abs(offset.y) > minimini(offset)) { offset.y = 0.0f; }
	if (glm::abs(offset.z) > minimini(offset)) { offset.z = 0.0f; }

	offset.x += glm::sign(offset.x) * 0.0001f;
	offset.y += glm::sign(offset.y) * 0.0001f;
	offset.z += glm::sign(offset.z) * 0.0001f;

	LOG_INFO(glm::to_string(offset));
	
	//offset = glm::vec3(0.0f);
	// Since no separating axis is found, the OBBs must be intersecting
	return 1;
}



void sixengine::CollisionSystem::BoxWithBox(Entity entityA, Entity entityB)
{
	OBB a, b;

	a.c = entityA.Component<Transform>()->getWorldPosition();

	a.u[0] = entityA.Component<Transform>()->GetRight();
	a.u[1] = entityA.Component<Transform>()->GetUpwards();
	a.u[2] = entityA.Component<Transform>()->GetForward();

	a.e = glm::vec3(0.5f);

	b.c = entityB.Component<Transform>()->getWorldPosition();

	b.u[0] = entityB.Component<Transform>()->GetRight();
	b.u[1] = entityB.Component<Transform>()->GetUpwards();
	b.u[2] = entityB.Component<Transform>()->GetForward();

	b.e = glm::vec3(0.5f);

	glm::vec3 totalOffset;
	float dist = 0.0f;

	if (TestOBBOBB(a, b, totalOffset))
	{
		LOG_INFO("Mamy to!");

		float portionA = 0.5f;
		float portionB = 0.5f;

		if (entityA.HasComponent<StaticBody>())
		{
			portionA = 0.0f;
			portionB = 1.0f;
		}
		else if (entityB.HasComponent<StaticBody>())
		{
			portionA = 1.0f;
			portionB = 0.0f;
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

	return;

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
		totalOffset.y = 0.0f;
		totalOffset.z = (back < front) ? (back) : (-front);

		if (abs(totalOffset.x) < abs(totalOffset.z)) { totalOffset.z = 0.0f; }
		else if (abs(totalOffset.x) > abs(totalOffset.z)) { totalOffset.x = 0.0f; }

		float portionA = 0.5f;
		float portionB = 0.5f;

		if (boxA->IsStatic() || boxB->IsStatic())
		{
			if (boxA->IsStatic() == false && boxB->IsStatic())
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (boxA->IsStatic() && boxB->IsStatic() == false)
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

		if (circleA->IsStatic() || circleB->IsStatic())
		{
			if (circleA->IsStatic() == false && circleB->IsStatic())
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (circleA->IsStatic() && circleB->IsStatic() == false)
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
	closestPoint.z = std::clamp(cirsclePos.z, boxPos.z - box->m_Size.z * 0.5f, boxPos.z + box->m_Size.z * 0.5f);

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

		if (box->IsStatic() || sphere->IsStatic())
		{
			if (box->IsStatic() && sphere->IsStatic())
			{
				portionA = 0.0f;
				portionB = 0.0f;
			}
			else if (box->IsStatic() == false && sphere->IsStatic())
			{
				portionA = 1.0f;
				portionB = 0.0f;
			}
			else if (box->IsStatic() && sphere->IsStatic() == false)
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

	m_EventManager->Emit<OnCollision>(OnCollision(collider, collision));
}

void sixengine::CollisionSystem::UpdateAll(EventManager& eventManager, float dt)
{
	m_EventManager = &eventManager;

	UpdateBoxesWithBoxes();
	UpdateSpheresWithSpheres();
	UpdateBoxesWithSpheres();
}

Entity* sixengine::CollisionSystem::CheckSphere(glm::vec3 center, float radius)
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
				return &spheres[i];

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
				return &boxes[i];
			}
		}
	}

	return NULL;	
}

std::vector<Entity*> sixengine::CollisionSystem::CheckSphereAll(glm::vec3 center, float radius)
{
	std::vector<Entity*> result;

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

				result.push_back(&spheres[i]);
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
				result.push_back(&boxes[i]);
			}
		}
	}

	return result;
}
