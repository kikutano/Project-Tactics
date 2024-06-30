#include "TranslateItem.h"

namespace tactics::component {

void TranslateItem::defineReflection() {
	componentReflection<TranslateItem>("translateItem")
		.data<&TranslateItem::speed>(hash("speed"))
		.data<&TranslateItem::axis>(hash("axis"));
}

void TranslateItemSystem::update(const ecs_view<Transform, TranslateItem>& view) {
	view.each([] (auto& transform, auto& translateItem) {
		transform.translate(translateItem.axis);
	});
}

}
