package com.example.weatherbackend.controller;

import com.example.weatherbackend.model.Reading;
import com.example.weatherbackend.repository.ReadingRepository;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.data.domain.Sort;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.List;

@RestController
@RequestMapping("/api/readings")
@CrossOrigin(origins = "*")
public class ReadingController {

    private final ReadingRepository readingRepository;

    public ReadingController(ReadingRepository readingRepository) {
        this.readingRepository = readingRepository;
    }

    @PostMapping
    public Reading createReading(@RequestBody Reading reading) {
        reading.setTimestamp(LocalDateTime.now());
        return readingRepository.save(reading);
    }

    @GetMapping("/latest")
    public ResponseEntity<Reading> latest() {
        Reading latest = readingRepository.findTopByOrderByTimestampDesc();
        if (latest == null) {
            return ResponseEntity.noContent().build();
        }
        return ResponseEntity.ok(latest);
    }

    @GetMapping
    public List<Reading> history(@RequestParam(required = false) String from,
                                 @RequestParam(required = false) String to,
                                 @RequestParam(defaultValue = "50") int limit) {
        Pageable pageable = PageRequest.of(0, limit, Sort.by("timestamp").descending());

        if (from != null && to != null) {
            LocalDateTime fromDt = LocalDateTime.parse(from);
            LocalDateTime toDt = LocalDateTime.parse(to);
            return readingRepository
                    .findByTimestampBetweenOrderByTimestampDesc(fromDt, toDt)
                    .stream()
                    .limit(limit)
                    .toList();
        }

        return readingRepository.findAll(pageable).getContent();
    }
}
