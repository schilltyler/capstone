

## Active Assignments 
```dataview
TABLE file.link as "Assignment", release as "Release Date", due as "Due Date"
FROM "Assignments"
WHERE date(release) <= date(today) AND date(due) >= date(today)
SORT due ASC
```

