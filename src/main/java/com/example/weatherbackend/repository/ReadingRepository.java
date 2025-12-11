package com.example.weatherbackend.repository;

import com.example.weatherbackend.model.Reading;
import org.springframework.data.jpa.repository.JpaRepository;

import java.time.LocalDateTime;
import java.util.List;

public interface ReadingRepository extends JpaRepository<Reading, Long> {
    Reading findTopByOrderByTimestampDesc();
    List<Reading> findByTimestampBetweenOrderByTimestampDesc(LocalDateTime from, LocalDateTime to);
}
