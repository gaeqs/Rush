//
// Created by gaelr on 5/10/23.
//

#ifndef NEON_RECTIFIED_IMPL_H
#define NEON_RECTIFIED_IMPL_H

#endif //NEON_RECTIFIED_IMPL_H


template<size_t Samples, typename Type, typename Curve, typename Allocator>
rush::RectifiedCurve<Samples, Type, Curve, Allocator>::
RectifiedCurve(const Curve& c) :
        curve(c) {
    populate();
}

template<size_t Samples, typename Type, typename Curve, typename Allocator>
rush::RectifiedCurve<Samples, Type, Curve, Allocator>::
RectifiedCurve(Curve&& c) :
        curve(std::move(c)) {
    populate();
}

template<size_t Samples, typename Type, typename Curve, typename Allocator>
void rush::RectifiedCurve<Samples, Type, Curve, Allocator>::
populate() {
    Type d = Type(1) / Samples;
    Type t = Type(0);
    Type l = Type(0);
    auto p = curve.fetch(t);

    // Calculate L
    for (size_t i = 0; i < Samples; ++i) {
        t += d;
        auto pn = curve.fetch(t);
        l += (pn - p).length();
        p = pn;
        samples[i] = l;
    }

    // Calculate T
    for (size_t i = 0; i < Samples; ++i) {
        samples[i] /= samples[Samples - 1];
    }
}


template<size_t Samples, typename Type, typename Curve, typename Allocator>
auto rush::RectifiedCurve<Samples, Type, Curve, Allocator>::
fetch(Type t) const {
    t *= Samples;

    auto index = std::clamp(static_cast<size_t>(t),
                            static_cast<size_t>(0), Samples - 1);

    Type n = index == 0 ? Type(0) : samples[index - 1];
    Type n1 = samples[index];

    Type offset = t - static_cast<Type>(index);

    return curve.fetch(n1 * offset + n * (Type(1) - offset));
}

template<size_t Samples, typename Type, typename Curve, typename Allocator>
auto rush::RectifiedCurve<Samples, Type, Curve, Allocator>::
fetchDerivative(Type t) const {
    t *= Samples;

    auto index = std::clamp(static_cast<size_t>(t),
                            static_cast<size_t>(0), Samples - 1);

    Type n = index == 0 ? Type(0) : samples[index - 1];
    Type n1 = samples[index];

    Type offset = t - static_cast<Type>(index);
    return curve.fetchDerivative(n1 * offset + n * (Type(1) - offset));
}
