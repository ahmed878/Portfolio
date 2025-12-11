import 'package:flutter/material.dart';
import '../services/api_service.dart';

class HistoryPage extends StatefulWidget {
  const HistoryPage({super.key});

  @override
  State<HistoryPage> createState() => _HistoryPageState();
}

class _HistoryPageState extends State<HistoryPage> {
  final _api = ApiService();
  List<dynamic> _readings = [];
  bool _loading = true;

  DateTime? _fromDate;
  DateTime? _toDate;

  @override
  void initState() {
    super.initState();
    _loadHistory();
  }

  Future<void> _loadHistory() async {
    setState(() => _loading = true);
    final data = await _api.getHistory(
      limit: 100,
      from: _fromDate,
      to: _toDate,
    );
    if (!mounted) return;
    setState(() {
      _readings = data;
      _loading = false;
    });
  }

  Future<void> _pickFromDate() async {
    final now = DateTime.now();
    final picked = await showDatePicker(
      context: context,
      initialDate: _fromDate ?? now,
      firstDate: DateTime(now.year - 1),
      lastDate: now,
    );
    if (picked != null) {
      setState(() => _fromDate = picked);
      await _loadHistory();
    }
  }

  Future<void> _pickToDate() async {
    final now = DateTime.now();
    final picked = await showDatePicker(
      context: context,
      initialDate: _toDate ?? now,
      firstDate: DateTime(now.year - 1),
      lastDate: now,
    );
    if (picked != null) {
      setState(() => _toDate = picked);
      await _loadHistory();
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('History')),
      body: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(8),
            child: Row(
              children: [
                Expanded(
                  child: OutlinedButton(
                    onPressed: _pickFromDate,
                    child: Text(
                      _fromDate == null
                          ? 'From date'
                          : 'From: ${_fromDate!.toLocal().toString().split(' ').first}',
                    ),
                  ),
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: OutlinedButton(
                    onPressed: _pickToDate,
                    child: Text(
                      _toDate == null
                          ? 'To date'
                          : 'To: ${_toDate!.toLocal().toString().split(' ').first}',
                    ),
                  ),
                ),
              ],
            ),
          ),
          Expanded(
            child: _loading
                ? const Center(child: CircularProgressIndicator())
                : ListView.builder(
                    itemCount: _readings.length,
                    itemBuilder: (context, index) {
                      final r = _readings[index];
                      return Card(
                        margin: const EdgeInsets.symmetric(
                            horizontal: 8, vertical: 4),
                        child: ListTile(
                          title: Text(
                            'T: ${r['temperature']} °C, H: ${r['humidity']} %',
                          ),
                          subtitle: Text(r['timestamp'].toString()),
                        ),
                      );
                    },
                  ),
          ),
        ],
      ),
    );
  }
}
