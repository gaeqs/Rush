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

    if (t < Type(0) || t > Type(1)) {
        return curve.fetch(t);
    }

    auto it = std::lower_bound(samples.cbegin(), samples.cend(), t);

    size_t next = std::distance(samples.cbegin(), it) + 1;
    size_t previous = next - 1;

    Type prevV = previous == 0 ? Type(0) : samples[previous - 1];
    Type nextV = samples[next - 1];

    Type percentage = (t - prevV) / (nextV - prevV);

    Type v = static_cast<Type>(previous) * (Type(1) - percentage) +
             static_cast<Type>(next) * percentage;

    return curve.fetch(v / Samples);
}

template<size_t Samples, typename Type, typename Curve, typename Allocator>
auto rush::RectifiedCurve<Samples, Type, Curve, Allocator>::
fetchDerivative(Type t) const {

    if (t < Type(0) || t > Type(1)) {
        return curve.fetch(t);
    }

    auto it = std::lower_bound(samples.cbegin(), samples.cend(), t);

    size_t next = std::distance(samples.cbegin(), it) + 1;
    size_t previous = next - 1;

    Type prevV = previous == 0 ? Type(0) : samples[previous - 1];
    Type nextV = samples[next - 1];

    Type percentage = (t - prevV) / (nextV - prevV);

    Type v = static_cast<Type>(previous) * (Type(1) - percentage) +
             static_cast<Type>(next) * percentage;

    return curve.fetchDerivative(v / Samples);
}
